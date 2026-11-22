// Vita3K emulator project
// Copyright (C) 2026 Vita3K team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#include "dlc.h"

#include <emuenv/state.h>
#include <mem/functions.h>
#include <mem/ptr.h>
#include <mem/state.h>
#include <util/log.h>

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <miniz.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <optional>
#include <vector>

namespace ss_delta {

namespace {

// the .pkh key: CRC-32/BZIP2 of the lower-case path (tools/lspk.py in ss-delta)
uint32_t path_hash(const std::string &path) {
    static const auto table = [] {
        std::array<uint32_t, 256> t{};
        for (uint32_t i = 0; i < 256; i++) {
            uint32_t c = i << 24;
            for (int k = 0; k < 8; k++)
                c = (c & 0x80000000) ? (c << 1) ^ 0x04C11DB7 : c << 1;
            t[i] = c;
        }
        return t;
    }();
    uint32_t c = 0xffffffff;
    for (const char ch : path) {
        const auto b = static_cast<uint8_t>(ch >= 'A' && ch <= 'Z' ? ch - 'A' + 'a' : ch);
        c = (c << 8) ^ table[((c >> 24) ^ b) & 0xff];
    }
    return c ^ 0xffffffff;
}

uint32_t be32(const uint8_t *p) {
    return (uint32_t(p[0]) << 24) | (uint32_t(p[1]) << 16) | (uint32_t(p[2]) << 8) | p[3];
}

std::vector<uint8_t> read_all(const fs::path &path) {
    fs::ifstream in(path, std::ios::binary);
    return std::vector<uint8_t>((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
}

// a member of an LSPK archive (<base>.pkh index, <base>.pk data: zlib, or stored when csize is 0)
std::optional<std::vector<uint8_t>> read_member(const fs::path &base, const std::string &path) {
    const auto pkh = read_all(fs::path(base).replace_extension(".pkh"));
    if (pkh.size() < 4)
        return std::nullopt;
    const uint32_t count = be32(pkh.data());
    if (pkh.size() < 4 + size_t(count) * 16)
        return std::nullopt;
    const uint32_t hash = path_hash(path);
    for (uint32_t i = 0; i < count; i++) {
        const uint8_t *r = pkh.data() + 4 + size_t(i) * 16;
        if (be32(r) != hash)
            continue;
        const uint32_t offset = be32(r + 4), size = be32(r + 8), csize = be32(r + 12);
        fs::ifstream pk(fs::path(base).replace_extension(".pk"), std::ios::binary);
        std::vector<uint8_t> stored(csize ? csize : size);
        pk.seekg(offset);
        if (!pk.read(reinterpret_cast<char *>(stored.data()), stored.size()))
            return std::nullopt;
        if (!csize)
            return stored;
        std::vector<uint8_t> data(size);
        mz_ulong out_size = size;
        if (mz_uncompress(data.data(), &out_size, stored.data(), csize) != MZ_OK || out_size != size)
            return std::nullopt;
        return data;
    }
    return std::nullopt;
}

// lines of a member, from archive_patch3 when it has it (it overrides archive, as in the game)
std::optional<std::vector<std::string>> read_lines(const fs::path &app_dir, const std::string &path) {
    const fs::path retail = app_dir / "resource" / "data" / "pack" / "mount" / "retail";
    for (const char *archive : { "archive_patch3", "archive" }) {
        if (const auto data = read_member(retail / archive, path)) {
            std::vector<std::string> lines;
            std::string line;
            for (const uint8_t c : *data) {
                if (c == '\n') {
                    lines.push_back(line);
                    line.clear();
                } else if (c != '\r')
                    line += static_cast<char>(c);
            }
            if (!line.empty())
                lines.push_back(line);
            return lines;
        }
    }
    return std::nullopt;
}

} // namespace

bool addon_data_present(const fs::path &app_dir, std::string &why) {
    // the add-on pacts are quest_param.csv rows up to 580, the raiments costume_table.csv rows up
    // to 130 (saves.cpp ADDON_LISTS)
    const auto quests = read_lines(app_dir, "resource/boot/quest_param.csv");
    if (!quests || quests->size() < 1 + 581) {
        why = quests ? "quest_param.csv has " + std::to_string(quests->size() - 1) + " quests, the add-ons need 581 (game update 1.30)"
                     : "quest_param.csv not found in the game's archives";
        return false;
    }
    const auto costumes = read_lines(app_dir, "resource/boot/costume_table.csv");
    const bool raiments = costumes && std::any_of(costumes->begin(), costumes->end(), [](const std::string &l) { return l.starts_with("130,"); });
    if (!raiments) {
        why = "costume_table.csv lacks the add-on raiments";
        return false;
    }
    return true;
}

// The gifts (ss-delta docs/01-research.md, "Content from the original game, and locked raiments").
// PSN told the game which add-ons were bought (sceNpAuthGetEntitlementIdList); Cassus's Claim Gift
// page (0x816ae350) then turned each one not yet received into a gift (0x816ae876) and gave it when
// taken (0x816b06e6...). Offline that page never gets past its download step. What taking a gift
// does is plain data in the save, so it is done here the same way:
//   * the add-on list is resource/database/network/entitlement_table[_us|_eu|_as].csv, the same
//     order in every region; a gift is a box of database/network/itembox_fixed.csv. Received gifts
//     are bytes, one per table row (0-based, the header skipped), at *0x81c9a160 + 0xc2ec8.
//   * *0x81c9a160 is the save file's image in memory (1 for received, 0 not), and the block
//     *0x81c98618 the game plays from (the image sits 0x8e3f0 after its start).
//   * raiments (COSTUME_nn: a first costume_table.csv row and a count): the game's grant (0x812f8834)
//     sets a row's state from 0 (hidden) or 4 (shown, not owned) to 1 (new; 3 once seen) in the
//     block, entries of 0x2c bytes from +0x6ab28 (the row, then the state at +0x14), the count at
//     +0x6ab24. The image keeps the same states, 8 bytes a row (row, state) from image + 0x84 +
//     0x522dc, copied into the block when the save is applied and back when it is written, so both
//     are set. A colour A (costume record: type at +0x4 below 0x1c and not 5, colour at +0xc 0) also
//     gives its three faction colours, 9 to 11 rows on. The count: up to 12 rows from the first; 13:
//     A to F and the faction colours; 14: G to I and the faction colours; 15: A and the faction
//     colours. The costume table: *0x81c9a28c, records of 0x5c bytes, *0x81d301d0 of them.
//   * hoods (PARTS_nn, an attach_param id below 0x80): a u16 at image + 0x84 + 0x568dc + 2 * id,
//     bit 0 for colour A and bit 15 for new (0x817808c8, 0x8178095e).
//   * sigils (STAMP_nn, a stamp id): a byte at image + 0x84 + 0x564dc + its index in the stamp
//     table (*0x81c9a3e4, records of 0xb4 bytes with the id at +0x4, *0x81d329b0 of them); bits 0,
//     1 and 2 set (0x81780708, 0x8178072a, 0x81780768).
// Each add-on is given if it isn't received or isn't owned, so a save loaded later, or the
// defaults the image holds before one is, get them too. Owned items are left alone.
namespace {

enum class GiftKind { Raiment,
    Hood,
    Sigil };

struct Gift {
    int row; // entitlement table row (index of its received byte), -1 for none
    const char *box;
    GiftKind kind;
    uint32_t id; // first costume row, hood id or stamp id
    uint32_t count; // raiments: rows, as above
    bool japan = false; // [game] japan_raiments
    uint32_t text = 0; // the original's raiments: the costume record's text id, there only with the mod
};

// the add-ons released outside Japan (DBG001 is the developers' box), then the Japan-only raiments:
// Sylvan (SSC018 and SSC020, marked never distributed in the other regions' tables) and Benkei, in
// no box (costume_table.csv row 168, its colour A as for its sibling Necky, 180), then the original
// game's collaboration raiments, rows the "original raiments" mod adds (ss-delta tools/og-raiments.py,
// D28), given when the game's costume table has them, colour A as in the original
constexpr std::array<Gift, 23> GIFTS{ {
    { 0, "COSTUME_01 Soulbinder", GiftKind::Raiment, 156, 13 },
    { 1, "PARTS_01 Fox Hood", GiftKind::Hood, 104, 1 },
    { 2, "STAMP_01 Fox Sigil", GiftKind::Sigil, 7101900, 1 },
    { 3, "COSTUME_02 Necky", GiftKind::Raiment, 180, 1 },
    { 4, "PARTS_07 Polytan Hood", GiftKind::Hood, 110, 1 },
    { 5, "PARTS_02 Demon's Horns", GiftKind::Hood, 105, 1 },
    { 6, "PARTS_03 Golden Mask", GiftKind::Hood, 106, 1 },
    { 7, "PARTS_04 Mind's Eyewear", GiftKind::Hood, 107, 1 },
    { 8, "PARTS_05 Wizard's Hat", GiftKind::Hood, 108, 1 },
    { 9, "PARTS_06 Magic Helm", GiftKind::Hood, 109, 1 },
    { 10, "PARTS_08 Possessed Mask", GiftKind::Hood, 111, 1 },
    { 11, "COSTUME_03 Slave", GiftKind::Raiment, 360, 1 },
    { 12, "COSTUME_04 Archsorcerer", GiftKind::Raiment, 372, 1 },
    { 13, "COSTUME_05 Mad Witch", GiftKind::Raiment, 528, 1 },
    { 14, "COSTUME_06 Easterner", GiftKind::Raiment, 252, 6 },
    { 15, "COSTUME_07 Saint", GiftKind::Raiment, 312, 6 },
    { 16, "COSTUME_08 Beast", GiftKind::Raiment, 324, 6 },
    { 19, "COSTUME_10 White Cat", GiftKind::Raiment, 600, 1 },
    { 18, "COSTUME_09 Sylvan", GiftKind::Raiment, 612, 15, true },
    { 20, "COSTUME_11 Sylvan (all colours)", GiftKind::Raiment, 612, 12, true },
    { -1, "Benkei", GiftKind::Raiment, 168, 1, true },
    { -1, "ARKS Armor (the original's)", GiftKind::Raiment, 720, 1, false, 3954 },
    { -1, "Dragonslayer (the original's)", GiftKind::Raiment, 732, 1, false, 3956 },
} };

constexpr Address SAVE_BLOCK_HOLDER = 0x81c98618, SAVE_IMAGE_HOLDER = 0x81c9a160;
constexpr Address COSTUME_TABLE_HOLDER = 0x81c9a28c, COSTUME_COUNT = 0x81d301d0;
constexpr Address STAMP_TABLE_HOLDER = 0x81c9a3e4, STAMP_COUNT = 0x81d329b0;
constexpr uint32_t IMAGE_SIZE = 0xef0e0; // the save file's size, also in the image's header (+0x8)
constexpr uint32_t RECEIVED = 0xc2ec8;
constexpr uint32_t IMAGE_DATA = 0x84; // what 0x8177e9da returns
constexpr uint32_t IMAGE_COSTUMES = 0x522dc, IMAGE_COSTUME_ROWS = 0x300;
constexpr uint32_t IMAGE_HOODS = 0x568dc, IMAGE_SIGILS = 0x564dc;
constexpr uint32_t BLOCK_COSTUME_COUNT = 0x6ab24, BLOCK_COSTUMES = 0x6ab28, BLOCK_COSTUME_SIZE = 0x2c, BLOCK_COSTUME_STATE = 0x14;
constexpr uint32_t COSTUME_RECORD = 0x5c, STAMP_RECORD = 0xb4;
constexpr uint32_t STATE_HIDDEN = 0, STATE_NEW = 1, STATE_SHOWN = 4;
constexpr uint32_t GAME_ROWS = 720; // costume_table.csv rows of 1.30; a mod may add more (up to 0x300)
constexpr uint32_t EMPTY_RECORD = 0xffffffff; // the image's raiment records past the table's rows
// the raiments worn, top and bottom (u16 costume rows), in three records of the image (found with the
// mod's Dragonslayer worn: 9 and 117 before, 732 and 732 after, in each)
constexpr std::array<uint32_t, 6> WORN = { 0x5b770, 0x5b774, 0x5bf96, 0x5bf9a, 0xeeb86, 0xeeb8a };

std::vector<uint32_t> raiment_rows(uint32_t first, uint32_t count) {
    std::vector<uint32_t> rows;
    const auto add = [&](uint32_t from, uint32_t n) {
        for (uint32_t i = 0; i < n; i++)
            rows.push_back(first + from + i);
    };
    if (count <= 12)
        add(0, count);
    else if (count == 13)
        add(0, 6);
    else if (count == 14)
        add(6, 3);
    else if (count == 15)
        add(0, 1);
    if (count >= 13 && count <= 15)
        add(9, 3);
    return rows;
}

class SaveView {
public:
    explicit SaveView(MemState &mem)
        : mem(mem) {}

    bool open() {
        image = holder(SAVE_IMAGE_HOLDER, RECEIVED + 0x20);
        const Address table = holder(COSTUME_TABLE_HOLDER, 0);
        const uint32_t *count = word(COSTUME_COUNT);
        costume_count = count && table && *count > 0 && *count <= IMAGE_COSTUME_ROWS ? *count : 0;
        costume_table = costume_count && valid(table, table + costume_count * COSTUME_RECORD) ? table : 0;
        if (!image || !costume_table || read<uint32_t>(image + 8) != IMAGE_SIZE)
            return false;
        // a whole save: at boot and around a load the game passes the image through half-built states
        // (its raiment records not yet numbered), which a load then replaces. A mod's rows have no
        // records in a save written before it (zeros), until the game writes the save again
        for (uint32_t row = 0; row < std::min(costume_count, GAME_ROWS); row++)
            if (read<uint32_t>(image + IMAGE_DATA + IMAGE_COSTUMES + row * 8) != row)
                return false;
        // the block, when it holds a costume table the size of the game's (it is applied from the
        // image some time after a load; until then it holds older states, which the load replaces)
        block = holder(SAVE_BLOCK_HOLDER, BLOCK_COSTUMES + IMAGE_COSTUME_ROWS * BLOCK_COSTUME_SIZE);
        if (block && read<uint32_t>(block + BLOCK_COSTUME_COUNT) != costume_count)
            block = 0;
        return true;
    }

    bool received(int row) const { return row < 0 || read<uint8_t>(image + RECEIVED + row); }
    void set_received(int row) {
        if (row >= 0)
            write<uint8_t>(image + RECEIVED + row, 1);
    }

    // a raiment's state in the image and, when it holds the table, the block; nullptr when the
    // row isn't where it should be. `create`: a mod's row without a record yet (an empty one, row
    // 0xffffffff, in a save written before the mod) gets one, as the game writes it (row, state)
    uint32_t *image_state(uint32_t row, bool create = false) {
        if (row >= IMAGE_COSTUME_ROWS)
            return nullptr;
        const Address entry = image + IMAGE_DATA + IMAGE_COSTUMES + row * 8;
        if (create && row >= GAME_ROWS && row < costume_count && read<uint32_t>(entry) == EMPTY_RECORD && read<uint32_t>(entry + 4) == 0)
            write<uint32_t>(entry, row);
        return read<uint32_t>(entry) == row ? word(entry + 4) : nullptr;
    }
    // the costume record's text id (the raiment's name), 0 when the table lacks the row
    uint32_t raiment_text(uint32_t row) const {
        if (!costume_table || row >= costume_count)
            return 0;
        const Address record = costume_table + row * COSTUME_RECORD;
        return read<uint32_t>(record) == row ? read<uint32_t>(record + 0x10) : 0;
    }
    uint32_t *block_state(uint32_t row) {
        if (!block || row >= costume_count)
            return nullptr;
        const Address entry = block + BLOCK_COSTUMES + row * BLOCK_COSTUME_SIZE;
        return read<uint32_t>(entry) == row ? word(entry + BLOCK_COSTUME_STATE) : nullptr;
    }
    static bool owned_state(const uint32_t *state) { return state && *state != STATE_HIDDEN && *state != STATE_SHOWN; }
    bool raiment_owned(uint32_t row) {
        const uint32_t *b = block_state(row);
        return owned_state(image_state(row)) && (!block || owned_state(b));
    }
    // 0x812f8834: returns whether the row was given now
    bool give_raiment(uint32_t row, bool faction_colours) {
        uint32_t *const in_image = image_state(row, true);
        uint32_t *const in_block = block_state(row);
        if (!in_image)
            return false;
        bool given = false;
        for (uint32_t *state : { in_image, in_block })
            if (state && !owned_state(state)) {
                *state = STATE_NEW;
                given = true;
            }
        if (given && faction_colours && costume_table && row < costume_count) {
            const Address record = costume_table + row * COSTUME_RECORD;
            const uint32_t type = read<uint32_t>(record + 4), colour = read<uint32_t>(record + 0xc);
            if (read<uint32_t>(record) == row && type < 0x1c && type != 5 && colour == 0)
                for (uint32_t extra = row + 9; extra <= row + 11; extra++)
                    give_raiment(extra, false);
        }
        return given;
    }

    // a raiment worn that the game's costume table lacks (a mod's, the mod since removed): the game
    // draws no body for it until another raiment is put on. Only reported: the game builds the model
    // as it reads the save, and a write into the image then races its own check of the file
    uint32_t worn_missing() const {
        uint32_t missing = 0;
        for (const uint32_t offset : WORN) {
            const uint16_t worn = read<uint16_t>(image + offset);
            if (worn >= costume_count && worn < IMAGE_COSTUME_ROWS)
                missing = worn;
        }
        return missing;
    }

    uint16_t *hood(uint32_t id) { return id < 0x80 ? Ptr<uint16_t>(image + IMAGE_DATA + IMAGE_HOODS + id * 2).get(mem) : nullptr; }
    uint8_t *sigil(uint32_t id) {
        const Address table = holder(STAMP_TABLE_HOLDER, 0);
        const uint32_t *count = word(STAMP_COUNT);
        if (!table || !count || *count > 0x1000 || !valid(table, table + *count * STAMP_RECORD))
            return nullptr;
        for (uint32_t i = 0; i < *count; i++)
            if (read<uint32_t>(table + i * STAMP_RECORD + 4) == id)
                return Ptr<uint8_t>(image + IMAGE_DATA + IMAGE_SIGILS + i).get(mem);
        return nullptr;
    }

    Address image = 0, block = 0;

private:
    bool valid(Address from, Address to) const { return from && is_valid_addr_range(mem, from, to); }
    uint32_t *word(Address a) const { return valid(a, a + 4) ? Ptr<uint32_t>(a).get(mem) : nullptr; }
    // the object a holder points to, when `size` bytes of it are mapped
    Address holder(Address a, uint32_t size) const {
        const uint32_t *p = word(a);
        return p && *p && valid(*p, *p + std::max<uint32_t>(size, 4)) ? *p : 0;
    }
    template <typename T>
    T read(Address a) const {
        T v{};
        std::memcpy(&v, Ptr<uint8_t>(a).get(mem), sizeof(T));
        return v;
    }
    template <typename T>
    void write(Address a, T v) { std::memcpy(Ptr<uint8_t>(a).get(mem), &v, sizeof(T)); }

    MemState &mem;
    Address costume_table = 0;
    uint32_t costume_count = 0;
};

} // namespace

void grant_addon_gifts(EmuEnvState &emuenv, GiftState &state, bool japan) {
    if (state.wait-- > 0)
        return;
    state.wait = 30; // about twice a second
    SaveView save(emuenv.mem);
    if (!save.open())
        return;
    if (const uint32_t worn = save.worn_missing(); worn && state.logged.insert(fmt::format("worn {:#x} {}", save.image, worn)).second)
        LOG_WARN("Add-ons: the raiment worn (row {}) isn't in the game's raiments (the original raiments mod removed?): "
                 "the body isn't drawn until another raiment is put on in Portraiture > Appearance > Raiments", worn);
    std::vector<std::string> given, missing;
    for (const Gift &gift : GIFTS) {
        if (gift.japan && !japan)
            continue;
        if (gift.text && save.raiment_text(gift.id) != gift.text)
            continue; // the mod isn't installed
        bool owned = false, done = false;
        switch (gift.kind) {
        case GiftKind::Raiment: {
            const auto rows = raiment_rows(gift.id, gift.count);
            owned = std::all_of(rows.begin(), rows.end(), [&](uint32_t row) { return save.raiment_owned(row); });
            if (!owned) {
                for (const uint32_t row : rows)
                    done |= save.give_raiment(row, true);
                owned = std::all_of(rows.begin(), rows.end(), [&](uint32_t row) { return save.raiment_owned(row); });
            }
            break;
        }
        case GiftKind::Hood:
            if (uint16_t *flags = save.hood(gift.id)) {
                owned = *flags & 1;
                if (!owned) {
                    *flags |= 0x8001;
                    owned = done = true;
                }
            }
            break;
        case GiftKind::Sigil:
            if (uint8_t *flags = save.sigil(gift.id)) {
                owned = *flags & 1;
                if (!owned) {
                    *flags |= 0x07;
                    owned = done = true;
                }
            }
            break;
        }
        if (!owned) {
            missing.push_back(gift.box);
            continue;
        }
        if (!save.received(gift.row)) {
            save.set_received(gift.row);
            done = true;
        }
        if (done)
            given.push_back(gift.box);
    }
    if (!given.empty())
        LOG_INFO("Add-ons: gave the PSN gifts {} in the save ({:#x}{})", fmt::join(given, ", "), save.image,
            save.block ? "" : ", the play block not yet holding it");
    if (!missing.empty() && state.logged.insert(fmt::format("{:#x} {}", save.image, fmt::join(missing, ","))).second)
        LOG_WARN("Add-ons: could not give {} (not where the save should have it)", fmt::join(missing, ", "));
}

} // namespace ss_delta
