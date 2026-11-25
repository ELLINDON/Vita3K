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

#pragma once

// rePatch mods, as the Vita's rePatch plugin loads them: files in ux0:rePatch/<title ID>/ are read in
// place of the same files in ux0:app/<title ID>/ (an undub's voice archives). They are installed
// beside the game, which stays as it is, and served through IOState::read_overrides when the game
// starts ([game] repatch, layout::repatch).

#include <util/fs.h>

#include <optional>
#include <string>
#include <vector>

struct EmuEnvState;

namespace game_frontend {

// --install-content with a folder or zip that holds rePatch/<title ID>/... (or is that folder): its
// files copied to <vita_fs>/ux0/rePatch/<title ID>/, merged with a mod there already. A mod made for
// one of the game's title IDs (title_ids, the first the one that runs) goes to the one installed,
// as for a game whose regions are the same build. nullopt: `path` holds no rePatch folder.
std::optional<bool> install_repatch(const fs::path &vita_fs, const fs::path &path, const std::vector<std::string> &title_ids);

// A folder or zip that also holds the game's own content (app/, patch/ or addcont/<title ID>/), as a
// dump that carries a rePatch mod beside the game does: install that content first, then the mod.
bool holds_game_content(const fs::path &path);

// The files of ux0/rePatch/<title_id>/ read in place of the game's own; returns how many. Files
// the game doesn't have are not added (the game's folder listings wouldn't show them): logged.
int apply_repatch(EmuEnvState &emuenv, const std::string &title_id);

} // namespace game_frontend
