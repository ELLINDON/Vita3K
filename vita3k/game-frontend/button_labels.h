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

// Xbox or keyboard button labels in a game's prompts: the game's textures with PlayStation buttons in
// them are redrawn as they are uploaded (renderer::TextureOverride), each symbol painted over with the
// letters of the gamepad button bound to it ([gamepad]; A, B, X, Y, LB, RB, LS, RS, and Xbox's Menu
// and View symbols for START and SELECT), or with a keycap holding the key bound to it ([keyboard], the
// first of two: X, C, F5, SPC, ENT, SHF, arrows ...). A game lists its textures (the hash of their data
// and where the symbols are) and sets the override on the texture cache.
//
// Two kinds of texture: pictures (UBC1/UBC3: icon atlases, tutorial pictures), where a symbol is light
// strokes in a dark ink blot and the letters come in their Xbox colours; and font pages (U8 with the
// 1RRR swizzle, the value is the glyph's coverage), where a symbol's cell gets a circled letter (or a
// keycap).

#include "input.h"
#include "settings.h"

#include <renderer/texture_cache.h>

#include <atomic>
#include <cstdint>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <tuple>
#include <vector>

struct CtrlState;

namespace game_frontend {

// the PlayStation symbols, and the words START and SELECT
enum class ButtonSymbol : uint8_t {
    Square,
    Circle,
    Cross,
    Triangle,
    L,
    R,
    Start,
    Select,
};

// a symbol's box in its texture, in the texture's pixels (as drawn: upright in a flip_y texture): the
// strokes (or the word), which are painted over; in a font page, the glyph's cell. white: the letters
// in white instead of their Xbox colours (a symbol on a disc of its own colour)
struct ButtonSlot {
    ButtonSymbol symbol;
    uint16_t x, y, w, h;
    bool white = false;
};

// a texture: FNV-1a 64 of its first level as the game holds it (swizzled), its base format
// (SceGxmTextureBaseFormat: UBC1, UBC3, or U8 for a font page), its size, a name for the log, and its
// symbols. flip_y: the game keeps the picture upside down (its rows bottom to top)
struct ButtonAtlas {
    uint64_t hash;
    uint32_t format;
    uint16_t width, height;
    const char *name;
    std::vector<ButtonSlot> slots;
    bool flip_y = false;
};

uint64_t fnv1a64(const uint8_t *data, size_t size);

// the [section] button_labels setting: playstation (the game's own icons), xbox, keyboard, or auto
SettingSpec button_labels_setting();

// button_labels = auto, by the device used last (`last`): "keyboard" after a key or a mouse button;
// otherwise the first connected gamepad's: "xbox" when it has lettered face buttons (`face` then gets
// their letters by position, south/east/west/north: a Nintendo-style pad's A is east), "playstation"
// for PlayStation symbols; with no gamepad "keyboard" (Android: "playstation", the on-screen controls)
std::string auto_button_style(const CtrlState &ctrl, std::map<std::string, std::string> &face,
    InputDevice last = InputDevice::None);

class ButtonLabels : public renderer::TextureOverride {
public:
    explicit ButtonLabels(std::vector<ButtonAtlas> atlases);

    // style: "xbox", "keyboard" or "playstation"; gamepad: [gamepad] (Vita button -> gamepad button);
    // scale: the pictures' size as a multiple of the textures' (the internal resolution, 1-4); face: the
    // face buttons' letters where they differ from an Xbox pad's (auto_button_style); keyboard:
    // [keyboard] (Vita button -> keys). Main thread. Returns true when the pictures change (then
    // refresh the texture cache's override).
    bool configure(const std::string &style, const std::map<std::string, std::string> &gamepad, int scale,
        const std::map<std::string, std::string> &face = {}, const std::map<std::string, std::string> &keyboard = {});

    bool wants(SceGxmTextureBaseFormat format, uint32_t width, uint32_t height) const override;
    bool make(SceGxmTextureBaseFormat format, uint32_t width, uint32_t height, const uint8_t *data, uint32_t size,
        std::vector<uint8_t> &rgba, uint32_t &out_width, uint32_t &out_height) override;

    struct Label {
        std::string text; // letters (A B L R S X Y) and Xbox's Menu and View; a key's name (key_cap_text)
        uint8_t r = 0, g = 0, b = 0;
        bool keycap = false; // a key: drawn in a keycap
    };

private:
    const std::vector<ButtonAtlas> m_atlases;
    std::set<std::tuple<uint32_t, uint16_t, uint16_t>> m_kinds; // format, width, height
    std::atomic<bool> m_enabled{ false };
    std::mutex m_mutex; // the configuration (main thread) and make (render thread)
    std::string m_style;
    std::map<std::string, std::string> m_gamepad, m_face, m_keyboard;
    int m_scale = 0;
    std::map<ButtonSymbol, Label> m_labels; // a symbol without one keeps its PlayStation icon
    std::set<uint64_t> m_logged;
};

} // namespace game_frontend
