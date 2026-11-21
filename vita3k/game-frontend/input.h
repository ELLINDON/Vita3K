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

// Keyboard and gamepad bindings from ss-delta.ini, and keyboard input for the SDL frontend (which,
// unlike the Qt interface, does not turn keys into Vita buttons itself).
#pragma once

#include "settings.h"

#include <input/physical_key.h>

#include <cstdint>
#include <functional>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

struct Config;
struct EmuEnvState;

namespace game_frontend {

// Applies [keyboard] and [gamepad] (Vita button -> keys / gamepad button) to the config; lines
// that can't be used are added to `warnings`.
void apply_bindings(Config &cfg, const std::map<std::string, std::string> &keyboard,
    const std::map<std::string, std::string> &gamepad, std::vector<std::string> &warnings);

// an SDL scancode as a physical key code (both are USB HID keyboard usages)
input::PhysicalKeyCode key_from_scancode(int scancode);
// up to two keys, comma separated, named as in [keyboard]; an unknown name adds "<setting>: unknown key"
// to `warnings`
std::vector<input::PhysicalKeyCode> parse_keys(const std::string &list, const std::string &setting, std::vector<std::string> &warnings);

// Keys held down, turned into buttons and stick axes in the controller's keyboard state
class Keyboard {
public:
    // an SDL scancode went down or up
    void key(EmuEnvState &emuenv, int scancode, bool pressed);
    // the window lost focus: release everything
    void release_all(EmuEnvState &emuenv);
    bool down(input::PhysicalKeyCode code) const { return pressed.contains(code); }

private:
    void update(EmuEnvState &emuenv);
    std::set<input::PhysicalKeyCode> pressed;
};

// How fast a game turns its camera with the right stick pushed along one axis, each axis on its own:
// (push 0-1, degrees per second) points, push increasing and speed never decreasing, the first at
// speed 0 (the end of the dead zone); between points the speed grows exponentially (linearly from
// the first). Measured per game (ss-delta: docs/01-research.md, "Mouse camera").
struct StickResponse {
    std::vector<std::pair<float, float>> points{ { 0.0f, 0.0f }, { 1.0f, 360.0f } };

    float max_speed() const { return points.back().second; }
    // the speed with the stick pushed `push`
    float speed(float push) const;
    // the least push that turns at `speed` (at most max_speed())
    float push_for(float speed) const;
};

// [mouse]: the mouse camera. While the frontend captures the cursor, mouse motion is the right stick.
// Each count of motion asks for a turn of degrees_per_count * sensitivity; each tick the stick is
// pushed as far as the game's StickResponse needs to turn what is still asked within about catch_up
// seconds, and what that push turns until the next tick is counted off, so the camera turns by as
// much as the mouse moved, at most at the game's fastest turn.
class MouseCamera {
public:
    struct Options {
        float sensitivity = 1.0f;
        bool invert_y = false;
    };
    static constexpr float degrees_per_count = 0.05f; // at sensitivity 1: 360 degrees in 7200 counts
    static constexpr float catch_up = 0.035f;
    // at most this many seconds of the fastest turn wait: what a flick asks beyond that is dropped
    static constexpr float max_backlog = 0.25f;

    void configure(const StickResponse &response, const Options &options);
    // relative motion of the captured mouse, in counts
    void motion(float dx, float dy);
    // each tick: the stick into the controller's mouse state (CtrlState::mouse_state)
    void update(EmuEnvState &emuenv, uint64_t now_ns);
    // the cursor is released: nothing more is asked, and the stick goes back to the centre
    void clear(EmuEnvState &emuenv);

private:
    struct Axis {
        float pending = 0; // degrees still to turn
        float speed = 0; // degrees per second the stick turns at since the last tick (signed)
    };
    float stick_for(Axis &axis, float dt) const;
    StickResponse response;
    Options options;
    Axis axes[2];
    uint64_t last_ns = 0;
    bool pushed = false; // the mouse state is off centre
};

// [touch]: fingers on the front touchscreen while their keys or gamepad button are held, and the
// ones the test driver holds through the input pipe
class Touches {
public:
    // a touch whose point moves (a game's HUD element): the point each frame in 960x544 screen
    // pixels, or none (no finger goes down while there is nothing to touch)
    using Target = std::function<std::optional<std::pair<float, float>>(EmuEnvState &)>;
    void configure(const std::vector<TouchBinding> &touches, std::vector<std::string> &warnings,
        const std::map<std::string, Target> &targets = {});
    // once a frame: the held fingers into the touch state
    void update(EmuEnvState &emuenv, const Keyboard &keyboard);

private:
    struct Binding {
        float x = 0, y = 0;
        std::vector<input::PhysicalKeyCode> keys;
        int gamepad_button = -1; // SDL_GamepadButton
        int gamepad_axis = -1; // SDL_GamepadAxis (a trigger)
        Target target; // when set, it gives the point instead of x, y
    };
    std::vector<Binding> bindings;
};

} // namespace game_frontend
