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

// The game's add-ons (costumes, pacts) ship in its own data since the last patch; PSN only
// recorded which were bought. This checks that the installed game has them.
#pragma once

#include <util/fs.h>

#include <set>
#include <string>

struct EmuEnvState;

namespace ss_delta {

// True when the installed game (app_dir = ux0/app/<title ID>) has the add-ons' pacts and raiments
// in its data; otherwise `why` says what is missing.
bool addon_data_present(const fs::path &app_dir, std::string &why);

// The add-ons that PSN handed out as gifts on Cassus's Claim Gift page (Soulbinder, the Necky
// raiment, the hoods ...), given as that page would, in the loaded save (GitHub issue #11); with
// `japan`, also the raiments only Japan had (Sylvan, Benkei). With the mods that add them, the
// original game's two raiments and its pre-order Spirits' offerings too. Also unlocks Blank Page's
// five slots that needed Memory Clods, a PlayStation Store consumable (GitHub issue #15), and takes
// a Memory Clod spend ("Sacrifice memory") as paid.
struct GiftState {
    int wait = 0; // ticks until the save is looked at again
    std::set<std::string> logged;
};
// [game] spirits_offerings: the original game's pre-order Spirits' offerings, which Delta keeps
// commented out in its tables, switched on (`on`) or off again with a packaged mod made from the
// installed game (title_id) in ux0/rePatch/; work_dir holds the package while it is made. Called
// before the rePatch mods are read.
void sync_spirits_offerings(const fs::path &vita_fs, const std::string &title_id, bool on, const fs::path &work_dir);

// Called every tick while [game] dlc is on.
void grant_addon_gifts(EmuEnvState &emuenv, GiftState &state, bool japan);

} // namespace ss_delta
