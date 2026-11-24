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

#include "../../game-frontend/packaged_mods.h"

#include <emuenv/state.h>
#include <mem/functions.h>
#include <mem/ptr.h>
#include <mem/state.h>
#include <util/log.h>
#include <util/string_utils.h>

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <miniz.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <map>
#include <optional>
#include <regex>
#include <set>
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

// The original game's pre-order "Spirits'" offerings (ss-delta docs/decisions.md D29). Delta keeps
// every row they need commented out (a ';' before the row) in its own tables; [game]
// spirits_offerings switches them on with a packaged mod made here from the installed game's own
// archives (nothing of the original game is needed): those rows appended without the ';' (with the
// continuation rows that follow them; GitHub issue #19: without the throws' four "投擲準備" rows the
// Spirit arrows had no arrows), with
//   * the two fang arms' punch-effect skill (1601001, 1601101), which Delta removed, set to 0, as on
//     Delta's own fang arms;
//   * the fang arms' punch motions (chara_motion_combo_param.csv, rows Delta left active), which
//     still name those removed skills: pointed at the arms' damage skill (1601003, 1601103), as
//     Delta's own fang arms' punches are (GitHub issue #18: the thrust crashed or missed);
//   * the falcon columns of arms rows that have 0 there taken from Delta's arm of the same model, as
//     the Spirit swords already have them;
//   * Spirits' Falconfeather (601800), "test" in Delta's western item text, named as in the original.
// grant_addon_gifts then gives each offering once a save.
namespace {

constexpr const char *SPIRITS_ID = "spirits-offerings";
constexpr const char *SPIRITS_VERSION = "3";
constexpr const char *SPIRITS_ARCHIVE = "resource/data/pack/mount/retail/archive_patch3";

struct SpiritTable {
    const char *member;
    const char *pattern; // the rows, after the ';'
    size_t rows; // in Delta 1.30
};
constexpr std::array<SpiritTable, 9> SPIRIT_TABLES{ {
    { "resource/boot/item_param_ply.csv", "60[01][0-9]00,", 20 },
    { "resource/boot/skill_param_ply.csv", "160[01][0-9]{3},", 49 },
    { "resource/boot/magic_eff_pack_param_ply.csv", "160[01][0-9]{3},", 49 },
    { "resource/boot/magic_explode_param_ply.csv", "160[01][0-9]{3},", 44 },
    { "resource/boot/magic_gimmick_param_ply.csv", "160[01][0-9]{3},", 12 },
    { "resource/boot/magic_bullet_param_ply.csv", "160[01][0-9]{3},", 5 },
    { "resource/boot/magic_crawl_param_ply.csv", "160[01][0-9]{3},", 6 },
    { "resource/boot/arms_param.csv", "ws160[01][0-9]{3},", 6 },
    { "resource/boot/protector_param.csv", "pa160[01][0-9]{3},", 2 },
} };
// arms_param.csv columns: model, falcon motion, falcon pierce skill, derived skills (normal, greed)
constexpr size_t ARMS_MODEL = 3, ARMS_FALCON_MOTION = 8, ARMS_PIERCE = 16, ARMS_DERIVED_NORMAL = 17, ARMS_DERIVED_GREED = 18;
// the fang arms' punch rows in chara_motion_combo_param.csv (motion, branch, level, skill, param),
// as Delta 1.30 has them, and as they are made
constexpr const char *COMBO_MEMBER = "resource/boot/chara_motion_combo_param.csv";
const std::array<std::pair<const char *, const char *>, 2> COMBO_FIXES{ {
    { "CH_ATD01_01_M,,1,1601001,1601001,", "CH_ATD01_01_M,,1,1601003,1601003," },
    { "CH_ATIW01_01_M,,1,1601101,1601101,", "CH_ATIW01_01_M,,1,1601103,1601103," },
} };
// the original's names of Spirits' Falconfeather in the western languages (Delta's say "test")
constexpr uint32_t FALCONFEATHER = 601800;
const std::array<std::pair<const char *, const char *>, 7> FALCONFEATHER_NAMES{ {
    { "us", "Spirits' Falconfeather" },
    { "ue", "Spirits' Falconfeather" },
    { "fr", "Don : Plume de faucon" },
    { "ge", "Geist-Falkenfeder" },
    { "it", "Piuma falco spettrale" },
    { "sp", "Pluma halc\xc3\xb3n esp\xc3\xadritu" },
    { "bp", "Pena de Esp\xc3\xadritos" },
} };

std::vector<std::string> split(const std::string &s, char sep) {
    std::vector<std::string> out;
    size_t start = 0;
    for (size_t i = 0; i <= s.size(); i++)
        if (i == s.size() || s[i] == sep) {
            out.push_back(s.substr(start, i - start));
            start = i + 1;
        }
    return out;
}

std::string join(const std::vector<std::string> &v, char sep) {
    std::string out;
    for (size_t i = 0; i < v.size(); i++) {
        if (i)
            out += sep;
        out += v[i];
    }
    return out;
}

void fix_arms(std::vector<std::string> &rows, const std::vector<std::string> &table) {
    std::map<std::string, std::vector<std::string>> by_model;
    for (const auto &l : table) {
        if (!l.starts_with("ws"))
            continue;
        auto c = split(l, ',');
        if (c.size() > ARMS_DERIVED_GREED && !c[ARMS_FALCON_MOTION].empty() && c[ARMS_FALCON_MOTION] != "0")
            by_model.try_emplace(c[ARMS_MODEL], c);
    }
    for (auto &row : rows) {
        auto c = split(row, ',');
        if (c.size() <= ARMS_DERIVED_GREED)
            continue;
        for (const size_t col : { ARMS_DERIVED_NORMAL, ARMS_DERIVED_GREED })
            if (c[col] == "1601001" || c[col] == "1601101")
                c[col] = "0";
        const auto own = by_model.find(c[ARMS_MODEL]);
        if (own != by_model.end() && (c[ARMS_FALCON_MOTION].empty() || c[ARMS_FALCON_MOTION] == "0")) {
            c[ARMS_FALCON_MOTION] = own->second[ARMS_FALCON_MOTION];
            if (c[ARMS_PIERCE].empty() || c[ARMS_PIERCE] == "0")
                c[ARMS_PIERCE] = own->second[ARMS_PIERCE];
        }
        row = join(c, ',');
    }
}

// chara_motion_combo_param.csv with the fang arms' punch rows fixed (COMBO_FIXES); false when the
// table doesn't have them as Delta 1.30 does
bool fixed_combo_table(const fs::path &retail, std::string &out) {
    std::optional<std::vector<uint8_t>> data;
    for (const char *archive : { "archive_patch3", "archive" })
        if ((data = read_member(retail / archive, COMBO_MEMBER)))
            break;
    if (!data)
        return false;
    out.assign(data->begin(), data->end());
    for (const auto &[from, to] : COMBO_FIXES) {
        const size_t at = out.find(from);
        if (at == std::string::npos || (at != 0 && out[at - 1] != '\n') || out.find(from, at + 1) != std::string::npos)
            return false;
        out.replace(at, std::strlen(from), to);
    }
    return true;
}

// an item text table (UTF-16 LE with a BOM, tab-separated, CRLF) with one item's name replaced;
// nullopt when it has no line for the item or already has that name
std::optional<std::vector<uint8_t>> renamed(const std::vector<uint8_t> &data, uint32_t item, const std::string &name) {
    if (data.size() < 2 || data.size() % 2)
        return std::nullopt;
    std::u16string text(data.size() / 2, u'\0');
    std::memcpy(text.data(), data.data(), data.size());
    const std::u16string key = string_utils::utf8_to_utf16(std::to_string(item) + "\t");
    const std::u16string want = string_utils::utf8_to_utf16(name);
    bool changed = false;
    for (size_t pos = 0; pos < text.size();) {
        size_t end = text.find(u"\r\n", pos);
        if (end == std::u16string::npos)
            end = text.size();
        const size_t start = text[pos] == u'﻿' ? pos + 1 : pos;
        if (text.compare(start, key.size(), key) == 0) {
            const size_t from = start + key.size();
            size_t to = text.find(u'\t', from);
            if (to == std::u16string::npos || to > end)
                to = end;
            if (text.compare(from, to - from, want) != 0) {
                text.replace(from, to - from, want);
                end = end + want.size() - (to - from);
                changed = true;
            }
        }
        pos = end + 2;
    }
    if (!changed)
        return std::nullopt;
    std::vector<uint8_t> out(text.size() * 2);
    std::memcpy(out.data(), text.data(), out.size());
    return out;
}

bool write_bytes(const fs::path &path, const std::string &data) {
    boost::system::error_code ec;
    fs::create_directories(path.parent_path(), ec);
    fs::ofstream out(path, std::ios::binary | std::ios::trunc);
    out.write(data.data(), static_cast<std::streamsize>(data.size()));
    return static_cast<bool>(out);
}

// the package (packaged_mods.h) in `dir`, made from the installed game's archives; false (and why)
// when they don't hold the commented-out rows Delta 1.30 has
bool make_spirits_package(const fs::path &app_dir, const fs::path &dir, std::string &why) {
    std::string appends;
    for (const auto &t : SPIRIT_TABLES) {
        const auto table = read_lines(app_dir, t.member);
        if (!table) {
            why = std::string(t.member) + " not found in the game's archives";
            return false;
        }
        const std::regex row(std::string("^") + t.pattern);
        // each row with the rows after it that have no ID (continuation rows: in
        // magic_eff_pack_param_ply.csv a pack's further effects, e.g. one per arrow of a throw).
        // Delta commented out only the first row of each, so the continuation rows stayed as they were
        std::vector<std::string> rows;
        size_t found = 0;
        for (size_t i = 0; i < table->size(); i++) {
            const auto &l = (*table)[i];
            if (std::regex_search(l, row)) {
                why = std::string(t.member) + " has them switched on already";
                return false;
            }
            if (l.size() > 1 && l[0] == ';' && std::regex_search(l.substr(1), row)) {
                rows.push_back(l.substr(1));
                found++;
                for (size_t j = i + 1; j < table->size() && (*table)[j].starts_with(','); j++)
                    rows.push_back((*table)[j]);
            }
        }
        if (found != t.rows) {
            why = fmt::format("{} has {} of the commented-out rows, not the {} of Delta 1.30", t.member, found, t.rows);
            return false;
        }
        const std::string member = t.member;
        const std::string name = member.substr(member.rfind('/') + 1);
        if (name == "arms_param.csv")
            fix_arms(rows, *table);
        std::string lines, conflicts;
        std::set<std::string> ids;
        for (const auto &r : rows) {
            lines += r + "\r\n";
            if (!r.starts_with(','))
                ids.insert(r.substr(0, r.find(',') + 1));
        }
        for (const auto &id : ids)
            conflicts += fmt::format("{}\"{}\"", conflicts.empty() ? "" : ", ", id);
        if (!write_bytes(dir / "tables" / name, lines)) {
            why = "could not write " + (dir / "tables" / name).string();
            return false;
        }
        appends += fmt::format("{}\n  {{\"member\": \"{}\", \"lines\": \"tables/{}\", \"conflicts\": [{}]}}",
            appends.empty() ? "" : ",", member, name, conflicts);
    }
    const fs::path retail = app_dir / "resource" / "data" / "pack" / "mount" / "retail";
    std::string combo;
    if (!fixed_combo_table(retail, combo)) {
        why = std::string(COMBO_MEMBER) + " lacks the fang arms' punch rows of Delta 1.30";
        return false;
    }
    if (!write_bytes(dir / "files" / fs_utils::utf8_to_path(COMBO_MEMBER), combo)) {
        why = std::string("could not write ") + COMBO_MEMBER;
        return false;
    }
    for (const auto &[lang, name] : FALCONFEATHER_NAMES) {
        const std::string member = fmt::format("resource/boot/{0}/item_param_text_{0}.msgt", lang);
        std::optional<std::vector<uint8_t>> data;
        for (const char *archive : { "archive_patch3", "archive" })
            if ((data = read_member(retail / archive, member)))
                break;
        if (!data)
            continue;
        if (const auto fixed = renamed(*data, FALCONFEATHER, name))
            if (!write_bytes(dir / "files" / fs_utils::utf8_to_path(member), std::string(fixed->begin(), fixed->end()))) {
                why = "could not write " + member;
                return false;
            }
    }
    const std::string manifest = fmt::format(
        "{{\n \"ss-delta-mod\": 1,\n \"id\": \"{}\",\n \"name\": \"The original Soul Sacrifice's Spirits' offerings\",\n"
        " \"version\": \"{}\",\n \"titles\": [\"PCSA00152\", \"PCSF00532\"],\n \"archive\": \"{}\",\n"
        " \"tables_from\": [\"resource/data/pack/mount/retail/archive\"],\n \"append\": [{}\n ],\n"
        " \"remove_note\": \"Without the mod the game doesn't show the Spirits' offerings, and drops them from the save when it next saves.\"\n}}\n",
        SPIRITS_ID, SPIRITS_VERSION, SPIRITS_ARCHIVE, appends);
    if (!write_bytes(dir / "ss-delta-mod.json", manifest)) {
        why = "could not write the manifest";
        return false;
    }
    return true;
}

} // namespace

void sync_spirits_offerings(const fs::path &vita_fs, const std::string &title_id, bool on, const fs::path &work_dir) {
    const fs::path installed = vita_fs / "ux0" / "rePatch" / title_id / game_frontend::PACKAGED_MODS_DIR / SPIRITS_ID;
    const auto manifest = fs::exists(installed / "ss-delta-mod.json") ? read_all(installed / "ss-delta-mod.json") : std::vector<uint8_t>{};
    const std::string current(manifest.begin(), manifest.end());
    if (!on) {
        if (!current.empty() && game_frontend::remove_packaged_mod(vita_fs, SPIRITS_ID, { title_id }))
            LOG_INFO("Spirits' offerings: switched off again ([game] spirits_offerings = false)");
        return;
    }
    if (current.find(fmt::format("\"version\": \"{}\"", SPIRITS_VERSION)) != std::string::npos)
        return; // installed, this version
    const fs::path dir = work_dir / (std::string(SPIRITS_ID) + ".new");
    boost::system::error_code ec;
    fs::remove_all(dir, ec);
    std::string why;
    if (!make_spirits_package(vita_fs / "ux0" / "app" / title_id, dir, why))
        LOG_WARN("Spirits' offerings: not switched on, {}", why);
    else if (game_frontend::install_packaged_mod(vita_fs, dir, { title_id }).value_or(false))
        LOG_INFO("Spirits' offerings: switched on (the game's own commented-out rows, merged into ux0/rePatch/{}); "
                 "with dlc, each is given once a save",
            title_id);
    else
        LOG_ERROR("Spirits' offerings: the packaged mod could not be installed (see above)");
    fs::remove_all(dir, ec);
}

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
//   * offerings (the original game's pre-order "Spirits'" offerings, items 600000-601900, which
//     Delta keeps commented out in its tables; ss-delta tools/spirits-offerings.py switches them on,
//     D29): the block's offering stock, built from the item tables when the save is applied, is a
//     count at +0x84 and entries of 0x2c bytes from +0x88 (the item id, four counts, one per star
//     rank, from +0x4, four flag words from +0x14: bit 1 obtained, bit 0 new). An offering box's
//     gift (0x816b05ca) calls 0x812f6ece(block, id, 0, 1): one more of rank 0 (at most 99) and the
//     obtained bit. The game writes the stock into the image when it saves (only ids it has rows
//     for: the image keeps none of them before), so only the block is set.
// Each add-on is given if it isn't received or isn't owned, so a save loaded later, or the
// defaults the image holds before one is, get them too. Owned items are left alone.
namespace {

enum class GiftKind { Raiment,
    Hood,
    Sigil,
    Offering };

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
// D28), given when the game's costume table has them, colour A as in the original; then the
// original game's pre-order offerings (D29), given when the game's item tables have them (the
// "Spirits' offerings" mod), once a save: the obtained bit stays when the offering is used up
// or carried (its count 0)
constexpr std::array<Gift, 43> GIFTS{ {
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
    { -1, "Spirits' Flamepike", GiftKind::Offering, 600000, 1 },
    { -1, "Spirits' Icebrand", GiftKind::Offering, 600100, 1 },
    { -1, "Spirits' Firearm", GiftKind::Offering, 600200, 1 },
    { -1, "Spirits' Stormfist", GiftKind::Offering, 600300, 1 },
    { -1, "Spirits' Flame Arrow", GiftKind::Offering, 600400, 1 },
    { -1, "Spirits' Stone Arrow", GiftKind::Offering, 600500, 1 },
    { -1, "Spirits' Blazestone", GiftKind::Offering, 600600, 1 },
    { -1, "Spirits' Blightstone", GiftKind::Offering, 600700, 1 },
    { -1, "Spirits' Greatroot", GiftKind::Offering, 600800, 1 },
    { -1, "Spirits' Sparktree", GiftKind::Offering, 600900, 1 },
    { -1, "Spirits' Dragon fang", GiftKind::Offering, 601000, 1 },
    { -1, "Spirits' Wolf fang", GiftKind::Offering, 601100, 1 },
    { -1, "Spirits' Heart", GiftKind::Offering, 601200, 1 },
    { -1, "Spirits' Flamebulb", GiftKind::Offering, 601300, 1 },
    { -1, "Spirits' Frostfluff", GiftKind::Offering, 601400, 1 },
    { -1, "Spirits' Storm Pinion", GiftKind::Offering, 601500, 1 },
    { -1, "Spirits' Storm Ward", GiftKind::Offering, 601600, 1 },
    { -1, "Spirits' Halebloom", GiftKind::Offering, 601700, 1 },
    { -1, "Spirits' Falconfeather", GiftKind::Offering, 601800, 1 },
    { -1, "Spirits' Gravelshield", GiftKind::Offering, 601900, 1 },
} };

constexpr Address SAVE_BLOCK_HOLDER = 0x81c98618, SAVE_IMAGE_HOLDER = 0x81c9a160;
constexpr Address COSTUME_TABLE_HOLDER = 0x81c9a28c, COSTUME_COUNT = 0x81d301d0;
constexpr Address STAMP_TABLE_HOLDER = 0x81c9a3e4, STAMP_COUNT = 0x81d329b0;
constexpr uint32_t IMAGE_SIZE = 0xef0e0; // the save file's size, also in the image's header (+0x8)
constexpr uint32_t IMAGE_CHECKSUM_KIND = 0x18, CHECKSUM_KIND = 0x22, IMAGE_CHECKSUM = 0x1c;
constexpr uint32_t RECEIVED = 0xc2ec8;
constexpr uint32_t IMAGE_DATA = 0x84; // what 0x8177e9da returns
constexpr uint32_t IMAGE_COSTUMES = 0x522dc, IMAGE_COSTUME_ROWS = 0x300;
constexpr uint32_t IMAGE_HOODS = 0x568dc, IMAGE_SIGILS = 0x564dc;
constexpr uint32_t BLOCK_COSTUME_COUNT = 0x6ab24, BLOCK_COSTUMES = 0x6ab28, BLOCK_COSTUME_SIZE = 0x2c, BLOCK_COSTUME_STATE = 0x14;
constexpr uint32_t COSTUME_RECORD = 0x5c, STAMP_RECORD = 0xb4;
constexpr uint32_t STATE_HIDDEN = 0, STATE_NEW = 1, STATE_SHOWN = 4;
constexpr uint32_t BLOCK_OFFERING_COUNT = 0x84, BLOCK_OFFERINGS = 0x88, BLOCK_OFFERING_SIZE = 0x2c;
constexpr uint32_t OFFERING_RANK0_COUNT = 0x4, OFFERING_RANK0_FLAGS = 0x14, OFFERINGS_MAX = 0x2000;
constexpr uint32_t OFFERING_NEW = 1, OFFERING_OBTAINED = 2, OFFERING_MOST = 99;
constexpr uint32_t GAME_ROWS = 720; // costume_table.csv rows of 1.30; a mod may add more (up to 0x300)
constexpr uint32_t EMPTY_RECORD = 0xffffffff; // the image's raiment records past the table's rows
// the raiments worn, top and bottom (u16 costume rows), in three records of the image (found with the
// mod's Dragonslayer worn: 9 and 117 before, 732 and 732 after, in each)
constexpr std::array<uint32_t, 6> WORN = { 0x5b770, 0x5b774, 0x5bf96, 0x5bf9a, 0xeeb86, 0xeeb8a };
// Blank Page (GitHub issue #15, ss-delta 01-research "Blank Page slots"): the slots that can hold a
// created pact page are a bitmask, bit n for slot n, a u32 at image + 0xc075c (0x81789d02) + 0x1da0 +
// 0xc. The screen has 8 slots; a new save has 0x7. The other five were unlocked with Memory Clods,
// a PlayStation Store consumable: on Yes in the unlock dialog (text 10062400) the game sets the bit
// (0x81024c14) and saves.
constexpr uint32_t BLANK_PAGE_SLOTS = 0xc2508, BLANK_PAGE_ALL = 0xff;
// A Memory Clod spend is a network task (0x81278238 makes it, 0x812782f4 runs it; vtable 0x81c5d004),
// at *0x81c953d0 while it lives: +0x44 its state, +0x48 1 once the clods are spent, +0x54 1 for a pact
// page made with "Sacrifice memory" and 3 for a slot (taken to be the clods needed), +0x58 and +0x5c the system
// texts of its dialogs (2007, "Insufficient Memory Clods", for both spends), +0x60 1 for the store page
// (START, "Add memory"). Offline it ends without +0x48; the screen that asked waits while the task lives
// and takes +0x48 = 1 as paid (0x81021838). Only state 11 (0x81278412) writes +0x48, so 1 set from
// here stays.
constexpr Address CLOD_TASK_HOLDER = 0x81c953d0, CLOD_TASK_VTABLE = 0x81c5d004;
constexpr uint32_t CLOD_TASK_SIZE = 0x68, CLOD_PAID = 0x48, CLOD_NEEDED = 0x54, CLOD_SHORT_TEXT = 0x58, CLOD_STORE = 0x60;
constexpr uint32_t CLOD_SHORT_TEXT_ID = 2007;

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
        image = holder(SAVE_IMAGE_HOLDER, IMAGE_SIZE);
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

    // an offering's entry in the block's stock (0 when the block isn't there or lacks the id)
    Address offering(uint32_t id) const {
        if (!block)
            return 0;
        const uint32_t count = read<uint32_t>(block + BLOCK_OFFERING_COUNT);
        if (count == 0 || count > OFFERINGS_MAX || !valid(block, block + BLOCK_OFFERINGS + count * BLOCK_OFFERING_SIZE))
            return 0;
        for (uint32_t i = 0; i < count; i++) {
            const Address entry = block + BLOCK_OFFERINGS + i * BLOCK_OFFERING_SIZE;
            if (read<uint32_t>(entry) == id)
                return entry;
        }
        return 0;
    }
    // the Blank Page slots the save has unlocked, nullptr when the mask holds bits past slot 7 (not
    // the mask, then)
    uint32_t *blank_page_slots() {
        uint32_t *slots = word(image + BLANK_PAGE_SLOTS);
        return slots && !(*slots & ~BLANK_PAGE_ALL) ? slots : nullptr;
    }

    // The image's checksum: +0x1c, CRC-32/BZIP2 of the whole image with it zeroed, when +0x18 is 0x22.
    // The game sets it when it writes the file (0x8178322a) and checks it after reading one
    // (0x81783248, from 0x8178ab88), once the player confirms "Loading complete"; the check leaves
    // it zeroed. A write into the image while that dialog is up failed the check ("Failed to load
    // save data.", the story-complete test save with gifts not yet in it, 2026-11-23).
    enum class Checksum { None, Valid, Other };
    Checksum checksum() const {
        const uint32_t stored = read<uint32_t>(image + IMAGE_CHECKSUM);
        if (stored == 0)
            return Checksum::None;
        if (read<uint32_t>(image + IMAGE_CHECKSUM_KIND) != CHECKSUM_KIND)
            return Checksum::Other;
        return image_crc() == stored ? Checksum::Valid : Checksum::Other;
    }
    void refresh_checksum() { write<uint32_t>(image + IMAGE_CHECKSUM, image_crc()); }

    bool offering_obtained(Address entry) const { return read<uint32_t>(entry + OFFERING_RANK0_FLAGS) & OFFERING_OBTAINED; }
    // 0x812f3c30 with one of rank 0, marked new
    void give_offering(Address entry) {
        write<uint32_t>(entry + OFFERING_RANK0_COUNT, std::min(read<uint32_t>(entry + OFFERING_RANK0_COUNT) + 1, OFFERING_MOST));
        write<uint32_t>(entry + OFFERING_RANK0_FLAGS, read<uint32_t>(entry + OFFERING_RANK0_FLAGS) | OFFERING_OBTAINED | OFFERING_NEW);
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
    // 0x81803dc2 over the image, its checksum field taken as 0 (the image was mapped whole in open())
    uint32_t image_crc() const {
        static const std::array<uint32_t, 256> table = [] {
            std::array<uint32_t, 256> t{};
            for (uint32_t i = 0; i < 256; i++) {
                uint32_t c = i << 24;
                for (int bit = 0; bit < 8; bit++)
                    c = c & 0x80000000u ? (c << 1) ^ 0x04c11db7u : c << 1;
                t[i] = c;
            }
            return t;
        }();
        const uint8_t *p = Ptr<uint8_t>(image).get(mem);
        uint32_t crc = 0xffffffffu;
        for (uint32_t i = 0; i < IMAGE_SIZE; i++) {
            const uint8_t byte = i - IMAGE_CHECKSUM < 4 ? 0 : p[i];
            crc = (crc << 8) ^ table[(crc >> 24) ^ byte];
        }
        return ~crc;
    }

    MemState &mem;
    Address costume_table = 0;
    uint32_t costume_count = 0;
};

} // namespace

void grant_addon_gifts(EmuEnvState &emuenv, GiftState &state, bool japan) {
    // every tick: a Memory Clod spend lives for a few frames
    MemState &mem = emuenv.mem;
    const auto mapped = [&](Address a, uint32_t size) { return a && is_valid_addr_range(mem, a, a + size); };
    if (mapped(CLOD_TASK_HOLDER, 4)) {
        const Address task = *Ptr<uint32_t>(CLOD_TASK_HOLDER).get(mem);
        if (mapped(task, CLOD_TASK_SIZE) && *Ptr<uint32_t>(task).get(mem) == CLOD_TASK_VTABLE) {
            const auto field = [&](uint32_t offset) { return Ptr<uint32_t>(task + offset).get(mem); };
            const int32_t needed = static_cast<int32_t>(*field(CLOD_NEEDED));
            if (*Ptr<uint8_t>(task + CLOD_STORE).get(mem) == 0 && *field(CLOD_SHORT_TEXT) == CLOD_SHORT_TEXT_ID && needed > 0
                && *field(CLOD_PAID) != 1) {
                *field(CLOD_PAID) = 1;
                LOG_INFO("Add-ons: {} Memory Clod{} taken as spent ({}; the PlayStation Store that sold them is closed)", needed,
                    needed == 1 ? "" : "s", needed == 1 ? "a pact page made with Sacrifice memory" : "a Blank Page slot");
            }
        }
    }
    if (state.wait-- > 0)
        return;
    state.wait = 30; // about twice a second
    SaveView save(emuenv.mem);
    if (!save.open())
        return;
    // a save just read (or written) carries its checksum, which the game checks after "Loading
    // complete": what is written into it here keeps the checksum right. A checksum that doesn't
    // match is an image the game is still filling or writing: left alone until it is done
    const SaveView::Checksum checksum = save.checksum();
    if (checksum == SaveView::Checksum::Other)
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
        const Address offering = gift.kind == GiftKind::Offering ? save.offering(gift.id) : 0;
        if (gift.kind == GiftKind::Offering && !offering)
            continue; // the mod isn't installed, or the block doesn't hold the save yet
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
        case GiftKind::Offering:
            owned = save.offering_obtained(offering);
            if (!owned) {
                save.give_offering(offering);
                owned = done = true;
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
    // the Blank Page slots that needed Memory Clods; the game keeps the mask when it saves
    if (uint32_t *slots = save.blank_page_slots()) {
        if (*slots != BLANK_PAGE_ALL) {
            *slots = BLANK_PAGE_ALL;
            given.push_back("the five Blank Page slots (Memory Clods)");
        }
    } else if (state.logged.insert(fmt::format("{:#x} blank page", save.image)).second)
        missing.push_back("the Blank Page slots");
    if (checksum == SaveView::Checksum::Valid && save.checksum() != SaveView::Checksum::Valid)
        save.refresh_checksum();
    if (!given.empty())
        LOG_INFO("Add-ons: gave {} in the save ({:#x}{})", fmt::join(given, ", "), save.image,
            save.block ? "" : ", the play block not yet holding it");
    if (!missing.empty() && state.logged.insert(fmt::format("{:#x} {}", save.image, fmt::join(missing, ","))).second)
        LOG_WARN("Add-ons: could not give {} (not where the save should have it)", fmt::join(missing, ", "));
}

} // namespace ss_delta
