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

// Packaged mods: a zip (or folder) with an ss-delta-mod.json, files to add to one of the game's LSPK
// archives (.pkh index, .pk data: Marvelous's format) and lines to append to tables (CSV) in it.
// A rePatch mod replaces whole files; a packaged mod is merged into the archive the game reads, its
// own or a rePatch mod's (an undub's), so mods that change one archive keep each other's changes, in
// any order. Installed into ux0/rePatch/<title ID>/: the merged archive, and the package itself in
// ss-delta-mods/<id>/ (not served to the game), from which it is merged again when another mod
// replaces that archive (refresh_packaged_mods, at install and when the game starts).
//
// ss-delta-mod.json:
//   {"ss-delta-mod": 1, "id": "<name, letters, digits, - _>", "name": "...", "version": "...",
//    "titles": ["<title IDs it is for>"],
//    "archive": "<the archive's path in the game's folder, without .pk/.pkh>",
//    "tables_from": ["<archives to read a table from when 'archive' lacks it>"],
//    "append": [{"member": "<the table's path in the archive>", "lines": "<file in the package>",
//                "conflicts": ["<the start of a line that means the rows are taken>"]}],
//    "remove_note": "<shown when it is removed>"}
// plus files/<path in the archive>: added, or replacing the archive's member of that path.

#include <util/fs.h>

#include <optional>
#include <string>
#include <vector>

namespace game_frontend {

constexpr const char *PACKAGED_MODS_DIR = "ss-delta-mods";

// --install-content with a zip or folder holding ss-delta-mod.json: merged and kept as above (a
// package of the same id replaced). nullopt: `path` is no packaged mod.
std::optional<bool> install_packaged_mod(const fs::path &vita_fs, const fs::path &path, const std::vector<std::string> &title_ids);

// --remove-mod <id>: taken out of the archive again; the archive is deleted when what is left is the
// game's own.
bool remove_packaged_mod(const fs::path &vita_fs, const std::string &id, const std::vector<std::string> &title_ids);

// Merges the installed packages of title_id again into archives that lack them (another mod
// installed over the archive, or the archive deleted).
void refresh_packaged_mods(const fs::path &vita_fs, const std::string &title_id);

} // namespace game_frontend
