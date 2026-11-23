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

#include "allies.h"

#include <emuenv/state.h>
#include <mem/state.h>
#include <util/log.h>

#include <cstring>

namespace ss_delta {

// The roster (ss-delta docs/01-research.md, "Sortiara as an ally"): the play block *0x81c98618 holds
// a count at +0x5c31c (495, one per boot/member_list.csv row) and entries of 0x1c bytes from
// +0x5c320: the member id (0x81a9f5c8: "ch9000_06" is 9000 * 100 + 6), the affinity, two words, a
// value, the flags and the member_list record. Flags: bit 1 a selectable ally, bit 0 new, bits 2 and
// 3 fallen or sacrificed (set over bit 1, cleared again by 0x812f764c). A story ally joins when the
// quest boot/member_add_list.csv names is cleared (0x812b031e): 0x812f737e then sets bits 0 and 1,
// keeps the affinity within 0..200 and zeroes the two words. Sortiara (ch9000_06) has everything an
// ally needs in the game's data but no row in that table; the game copies the block into the save
// when it writes it.
namespace {

constexpr Address SAVE_BLOCK_HOLDER = 0x81c98618;
constexpr uint32_t MEMBER_COUNT = 0x5c31c, MEMBERS = 0x5c320, MEMBER_SIZE = 0x1c, MEMBERS_MAX = 0x200;
constexpr uint32_t MEMBER_AFFINITY = 0x4, MEMBER_WORD1 = 0x8, MEMBER_WORD2 = 0xc, MEMBER_FLAGS = 0x14;
constexpr uint32_t FLAG_NEW = 1, FLAG_ALLY = 2, FLAG_FALLEN = 4 | 8;
constexpr int32_t AFFINITY_MAX = 200;
constexpr uint32_t SORTIARA = 900006, SORTIARA_INDEX = 6;

} // namespace

void add_sortiara_ally(EmuEnvState &emuenv, AllyState &state) {
    if (state.wait-- > 0)
        return;
    state.wait = 30; // about twice a second
    MemState &mem = emuenv.mem;
    const auto mapped = [&](Address a, uint32_t size) { return a && is_valid_addr_range(mem, a, a + size); };
    const auto word = [&](Address a) { return Ptr<uint32_t>(a).get(mem); };
    if (!mapped(SAVE_BLOCK_HOLDER, 4))
        return;
    const Address block = *word(SAVE_BLOCK_HOLDER);
    if (!mapped(block, MEMBERS + MEMBERS_MAX * MEMBER_SIZE))
        return;
    const uint32_t count = *word(block + MEMBER_COUNT);
    if (count <= SORTIARA_INDEX || count > MEMBERS_MAX)
        return;
    Address entry = block + MEMBERS + SORTIARA_INDEX * MEMBER_SIZE;
    if (*word(entry) != SORTIARA) {
        // a member_list.csv in another order (a mod's)
        entry = 0;
        for (uint32_t i = 0; i < count && !entry; i++)
            if (*word(block + MEMBERS + i * MEMBER_SIZE) == SORTIARA)
                entry = block + MEMBERS + i * MEMBER_SIZE;
        if (!entry)
            return;
    }
    uint32_t &flags = *word(entry + MEMBER_FLAGS);
    // once a save: an ally already, or one who fell (and stays fallen until revived), is left alone
    if (flags & (FLAG_ALLY | FLAG_FALLEN))
        return;
    int32_t affinity = static_cast<int32_t>(*word(entry + MEMBER_AFFINITY));
    affinity = affinity > AFFINITY_MAX ? AFFINITY_MAX : affinity < 0 ? 0 : affinity;
    *word(entry + MEMBER_AFFINITY) = static_cast<uint32_t>(affinity);
    *word(entry + MEMBER_WORD1) = 0;
    *word(entry + MEMBER_WORD2) = 0;
    flags |= FLAG_NEW | FLAG_ALLY;
    // at the title (the block's defaults) and once a save is loaded, until it is written with her
    LOG_INFO("Allies: Sortiara joined the allies in the save in play (block {:#x}, affinity {})", block, affinity);
}

} // namespace ss_delta
