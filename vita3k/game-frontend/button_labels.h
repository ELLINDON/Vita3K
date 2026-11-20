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

// Xbox button labels in a game's prompts: the game's button-icon atlases are redrawn as they are
// uploaded (renderer::TextureOverride), each PlayStation symbol painted over with the letters of the
// gamepad button bound to it ([gamepad]; A, B, X, Y, LB, RB, LS, RS). A game lists its atlases (the
// hash of their data and where the symbols are) and sets the override on the texture cache.

#include "settings.h"

#include <renderer/texture_cache.h>

#include <atomic>
#include <cstdint>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <vector>

namespace game_frontend {

// the PlayStation symbols in the atlases
enum class ButtonSymbol : uint8_t {
    Square,
    Circle,
    Cross,
    Triangle,
    L,
    R,
};

// a symbol's box in its atlas, in the atlas's pixels: the white strokes, which are painted over
struct ButtonSlot {
    ButtonSymbol symbol;
    uint16_t x, y, w, h;
};

// an atlas: FNV-1a 64 of its first level as the game holds it (swizzled UBC3), its size, a name for
// the log, and its symbols
struct ButtonAtlas {
    uint64_t hash;
    uint16_t width, height;
    const char *name;
    std::vector<ButtonSlot> slots;
};

uint64_t fnv1a64(const uint8_t *data, size_t size);

// the [section] button_labels setting: playstation (the game's own icons) or xbox
SettingSpec button_labels_setting();

class ButtonLabels : public renderer::TextureOverride {
public:
    explicit ButtonLabels(std::vector<ButtonAtlas> atlases);

    // style: the setting's value; gamepad: [gamepad] (Vita button -> gamepad button); scale: the
    // pictures' size as a multiple of the atlases' (the internal resolution, 1-4). Main thread.
    // Returns true when the pictures change (then refresh the texture cache's override).
    bool configure(const std::string &style, const std::map<std::string, std::string> &gamepad, int scale);

    bool wants(SceGxmTextureBaseFormat format, uint32_t width, uint32_t height) const override;
    bool make(SceGxmTextureBaseFormat format, uint32_t width, uint32_t height, const uint8_t *data, uint32_t size,
        std::vector<uint8_t> &rgba, uint32_t &out_width, uint32_t &out_height) override;

    struct Label {
        std::string text; // one or two of A B L R S X Y
        uint8_t r = 0, g = 0, b = 0;
    };

private:
    const std::vector<ButtonAtlas> m_atlases;
    std::set<std::pair<uint16_t, uint16_t>> m_sizes;
    std::atomic<bool> m_enabled{ false };
    std::mutex m_mutex; // the configuration (main thread) and make (render thread)
    std::string m_style;
    std::map<std::string, std::string> m_gamepad;
    int m_scale = 0;
    std::map<ButtonSymbol, Label> m_labels; // a symbol without one keeps its PlayStation icon
    std::set<uint64_t> m_logged;
};

} // namespace game_frontend
