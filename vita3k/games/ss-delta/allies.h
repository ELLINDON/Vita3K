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

// Allies the game has every piece of but never lets the player pick.
#pragma once

#include <mem/ptr.h>

struct EmuEnvState;

namespace ss_delta {

struct AllyState {
    int wait = 0; // ticks until the roster is looked at again
};

// [game] sortiara_ally: Sortiara, who fights beside the player in the early story quests, joins the
// allies in the loaded save, once, as a story ally joins after a quest (ss-delta docs/01-research.md,
// "Sortiara as an ally"). Called every tick while the setting is on.
void add_sortiara_ally(EmuEnvState &emuenv, AllyState &state);

} // namespace ss_delta
