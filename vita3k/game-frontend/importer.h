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

// A game's release is its own folder: the executable, its files, and the importer in setup/ (the
// recompiler kit's setup.py with a Python of its own), which builds the game from the player's dump.
// The executable starts the importer when the game isn't built yet, or when asked (--install), and
// waits for it (ss-delta D22).

#include <util/fs.h>

#include <string>
#include <vector>

namespace game_frontend {

// the running executable's path (macOS: inside its app bundle)
fs::path own_executable();

// Windows: write to the console of the program that started this one (a command prompt, a
// launcher), so that --help and the importer's console messages are seen. Does nothing elsewhere,
// or without such a console.
void attach_parent_console();

// macOS: Gatekeeper runs a downloaded app the player let through ("Open Anyway") from a read-only,
// translocated copy that holds only the app. Such an app clears the download's quarantine from its
// folder and starts itself again from there (its files beside it, writable). True: this process is
// to exit. Elsewhere, or not translocated: false.
bool relaunch_from_original_location(int argc, char *argv[]);

// macOS: removes the download's quarantine (com.apple.quarantine) from the folder's files, so
// Gatekeeper doesn't stop the importer's Python and the libraries it loads. Does nothing elsewhere.
void clear_quarantine(const fs::path &dir);

// true if the folder holds an importer (setup/kit/tools/setup.py and its Python)
bool has_importer(const fs::path &game_dir);

// Runs the importer with `args` (its parts, --add-to-steam ...) and waits; console: its console
// flow (Windows: python.exe instead of the windowless pythonw.exe). Its exit code: 0 built (play),
// 2 built but its window closed without Play, 1 failed or cancelled; -1: it could not be started.
int run_importer(const fs::path &game_dir, const fs::path &exe, const std::vector<std::string> &args, bool console);

} // namespace game_frontend
