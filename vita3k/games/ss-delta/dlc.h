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
// `japan`, also the raiments only Japan had (Sylvan, Benkei).
struct GiftState {
    int wait = 0; // ticks until the save is looked at again
    std::set<std::string> logged;
};
// Called every tick while [game] dlc is on.
void grant_addon_gifts(EmuEnvState &emuenv, GiftState &state, bool japan);

} // namespace ss_delta
