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

// re-nova.exe: Phantasy Star Nova (PCSG00351, update 1.05) on the shared single-game frontend
// (game-frontend/profile.h). The re-nova project's docs describe the port.
//
// Phase 0/1 of that project: the game's profile and settings file only, so the game can be booted,
// dumped (--dump-elfs) and studied with a log that is flushed on close. Its saves, controls (the
// game's PS TV mode), the English patch check and the rendering settings come in later phases.

#include "../../game-frontend/profile.h"

#include <config/state.h>
#include <emuenv/state.h>
#include <util/log.h>

#include <cstdlib>

// provides WinMain for the windowed executable, which calls main below
#include <SDL3/SDL_main.h>

namespace {

namespace layout = game_frontend::layout;

constexpr const char *TITLE_ID = "PCSG00351";

game_frontend::IniLayout ini_layout() {
    const std::string id = "re-nova", exe = id + game_frontend::executable_suffix;
    return {
        "Re:NOVA settings. Lines starting with ; are comments; delete the ; to use a line.",
        {
            layout::paths(TITLE_ID, id, exe),
            { "display", "",
                { layout::scale(), layout::window_scale(), layout::fullscreen(id, exe), layout::vsync(),
                    layout::filter(), layout::mipmaps(false) } },
            layout::audio(),
            { "controls", "",
                { { "pstv_mode", game_frontend::SettingSpec::Kind::Bool, "false", 0, 0, {},
                    "Run as a PS TV, where the game takes a DualShock's L2/R2 and L3/R3 and has no\n"
                    "touchscreens. Being studied. Read when the game starts." } } },
            layout::recompiled(id),
            layout::debug(id),
            layout::keyboard(),
            layout::gamepad(),
            layout::touch("; None yet.\n"),
        },
    };
}

} // namespace

int main(int argc, char *argv[]) {
    game_frontend::Profile profile;
    profile.title_id = TITLE_ID;
    profile.name = "Re:NOVA";
    profile.id = "re-nova";
    profile.ini = ini_layout();
    profile.hooks.configure = [](game_frontend::Context &ctx, Config &cfg) {
        // a PS TV: pads on the external ports, the PS TV model (SceCtrl, SceSysmem)
        cfg.pstv_mode = ctx.settings->values.get_bool("controls.pstv_mode");
        // RENOVA_MEMORY_MAPPING (debug, Phase 0/4): another memory mapping than the front end's
        // double-buffer, to study the player character drawn black (community: external-host)
        if (const char *mapping = std::getenv("RENOVA_MEMORY_MAPPING"); mapping && *mapping) {
            cfg.memory_mapping = mapping;
            LOG_INFO("memory mapping (RENOVA_MEMORY_MAPPING): {}", cfg.memory_mapping);
        }
    };
    return game_frontend::run(profile, argc, argv);
}
