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

#include "button_labels.h"

#include <gxm/types.h>
#include <renderer/functions.h>
#include <util/log.h>

#include <algorithm>
#include <cctype>
#include <cmath>

namespace game_frontend {

namespace {

// The letters, drawn as strokes of one width (no font): x right, y down, in units of the letter's
// height; arcs by their centre, radius and angles in degrees (0 right, 90 down). Like the game's own
// symbols, which are white strokes in an ink blot.
struct Stroke {
    bool arc;
    float a, b, c, d, e; // line: (a, b)-(c, d); arc: centre (a, b), radius c, angles d..e
};
struct Glyph {
    float width;
    std::vector<Stroke> strokes;
};

constexpr float STROKE = 0.19f; // the strokes' width
constexpr float T = STROKE / 2, B = 1 - STROKE / 2, M = 0.5f; // the strokes' top, bottom and middle
constexpr float BOWL = (M - T) / 2; // the radius of B's, R's and S's bowls
constexpr float GAP = 0.1f; // between two letters

Stroke line(float x0, float y0, float x1, float y1) { return { false, x0, y0, x1, y1, 0 }; }
Stroke arc(float cx, float cy, float r, float a0, float a1) { return { true, cx, cy, r, a0, a1 }; }

const Glyph *glyph(char c) {
    static const std::map<char, Glyph> glyphs = [] {
        std::map<char, Glyph> g;
        const float ax = 0.35f; // A's apex
        g['A'] = { 2 * ax, { line(T, B, ax, T), line(ax, T, 2 * ax - T, B), line(0.18f, 0.64f, 2 * ax - 0.18f, 0.64f) } };
        const float b1 = 0.30f, b2 = 0.33f; // where B's straight parts turn into its bowls
        g['B'] = { b2 + BOWL + T, { line(T, T, T, B), line(T, T, b1, T), arc(b1, T + BOWL, BOWL, -90, 90), line(b1, M, T, M), line(T, M, b2, M), arc(b2, M + BOWL, BOWL, -90, 90), line(b2, B, T, B) } };
        g['X'] = { 0.68f, { line(T, T, 0.68f - T, B), line(0.68f - T, T, T, B) } };
        g['Y'] = { 0.68f, { line(T, T, 0.34f, M), line(0.68f - T, T, 0.34f, M), line(0.34f, M, 0.34f, B) } };
        g['L'] = { 0.56f, { line(T, T, T, B), line(T, B, 0.56f - T, B) } };
        g['R'] = { 0.64f, { line(T, T, T, B), line(T, T, b1, T), arc(b1, T + BOWL, BOWL, -90, 90), line(b1, M, T, M), line(0.28f, M, 0.64f - T, B) } };
        const float sx = 0.31f; // S's bowls' centres
        g['S'] = { sx + BOWL + T, { arc(sx, T + BOWL, BOWL, 90, 330), arc(sx, M + BOWL, BOWL, -90, 150) } };
        return g;
    }();
    const auto it = glyphs.find(c);
    return it == glyphs.end() ? nullptr : &it->second;
}

float segment_distance(float px, float py, float ax, float ay, float bx, float by) {
    const float vx = bx - ax, vy = by - ay, wx = px - ax, wy = py - ay;
    const float len = vx * vx + vy * vy;
    const float t = len > 0 ? std::clamp((wx * vx + wy * vy) / len, 0.f, 1.f) : 0.f;
    return std::hypot(wx - t * vx, wy - t * vy);
}

float stroke_distance(const Stroke &s, float px, float py) {
    if (!s.arc)
        return segment_distance(px, py, s.a, s.b, s.c, s.d);
    constexpr float pi = 3.14159265f;
    float angle = std::atan2(py - s.b, px - s.a) * 180 / pi;
    while (angle < s.d)
        angle += 360;
    while (angle >= s.d + 360)
        angle -= 360;
    if (angle <= s.e)
        return std::abs(std::hypot(px - s.a, py - s.b) - s.c);
    const auto end = [&](float deg) {
        return std::hypot(px - (s.a + s.c * std::cos(deg * pi / 180)), py - (s.b + s.c * std::sin(deg * pi / 180)));
    };
    return std::min(end(s.d), end(s.e));
}

float text_width(const std::string &text) {
    float w = 0;
    for (const char c : text)
        if (const Glyph *g = glyph(c))
            w += g->width;
    return w + GAP * (text.size() > 1 ? text.size() - 1 : 0);
}

// draws `label` centred on (cx, cy), `height` pixels tall, into an RGBA image
void draw_text(std::vector<uint8_t> &img, uint32_t w, uint32_t h, float cx, float cy, float height, const ButtonLabels::Label &label) {
    float x = cx - text_width(label.text) * height / 2;
    const float top = cy - height / 2;
    for (const char c : label.text) {
        const Glyph *g = glyph(c);
        if (!g)
            continue;
        const int x0 = std::max(0, static_cast<int>(std::floor(x)) - 1);
        const int x1 = std::min<int>(w, static_cast<int>(std::ceil(x + g->width * height)) + 1);
        const int y0 = std::max(0, static_cast<int>(std::floor(top)) - 1);
        const int y1 = std::min<int>(h, static_cast<int>(std::ceil(top + height)) + 1);
        for (int py = y0; py < y1; py++) {
            for (int px = x0; px < x1; px++) {
                // the pixel's centre in the letter's units
                const float u = (px + 0.5f - x) / height, v = (py + 0.5f - top) / height;
                float d = 1e9f;
                for (const auto &s : g->strokes)
                    d = std::min(d, stroke_distance(s, u, v));
                const float cover = std::clamp((STROKE / 2 - d) * height + 0.5f, 0.f, 1.f);
                if (cover <= 0)
                    continue;
                uint8_t *p = &img[(static_cast<size_t>(py) * w + px) * 4];
                p[0] = static_cast<uint8_t>(std::lround(p[0] + (label.r - p[0]) * cover));
                p[1] = static_cast<uint8_t>(std::lround(p[1] + (label.g - p[1]) * cover));
                p[2] = static_cast<uint8_t>(std::lround(p[2] + (label.b - p[2]) * cover));
                p[3] = std::max<uint8_t>(p[3], static_cast<uint8_t>(std::lround(cover * 255)));
            }
        }
        x += (g->width + GAP) * height;
    }
}

// the label a gamepad button ([gamepad]'s names, SDL's) shows; empty text: none
ButtonLabels::Label label_of(std::string button) {
    std::transform(button.begin(), button.end(), button.begin(), [](unsigned char c) { return std::tolower(c); });
    struct Entry {
        const char *name, *text;
        uint8_t r, g, b;
    };
    // the face buttons in the colours of their letters on an Xbox pad, the rest white
    static const Entry entries[] = {
        { "south", "A", 110, 200, 70 },
        { "a", "A", 110, 200, 70 },
        { "east", "B", 235, 75, 60 },
        { "b", "B", 235, 75, 60 },
        { "west", "X", 70, 150, 240 },
        { "x", "X", 70, 150, 240 },
        { "north", "Y", 245, 200, 50 },
        { "y", "Y", 245, 200, 50 },
        { "leftshoulder", "LB", 240, 240, 240 },
        { "rightshoulder", "RB", 240, 240, 240 },
        { "leftstick", "LS", 240, 240, 240 },
        { "rightstick", "RS", 240, 240, 240 },
    };
    for (const auto &e : entries)
        if (button == e.name)
            return { e.text, e.r, e.g, e.b };
    return {};
}

std::string trim(const std::string &s) {
    const auto b = s.find_first_not_of(" \t");
    const auto e = s.find_last_not_of(" \t");
    return b == std::string::npos ? std::string() : s.substr(b, e - b + 1);
}

} // namespace

uint64_t fnv1a64(const uint8_t *data, size_t size) {
    uint64_t h = 0xcbf29ce484222325ull;
    for (size_t i = 0; i < size; i++)
        h = (h ^ data[i]) * 0x100000001b3ull;
    return h;
}

SettingSpec button_labels_setting() {
    SettingSpec s{ "button_labels", SettingSpec::Kind::Choice, "playstation", 0, 0, { "playstation", "xbox" },
        "The buttons the game's prompts show: playstation (the game's own icons) or xbox (A, B, X, Y,\n"
        "LB, RB: the gamepad button that [gamepad] binds to each Vita button). For now the face buttons\n"
        "and L/R; the D-pad, sticks, START/SELECT and tutorial pictures keep their PlayStation icons." };
    s.live = true;
    s.title = "Button prompts";
    s.choice_labels = { "PlayStation", "Xbox" };
    return s;
}

ButtonLabels::ButtonLabels(std::vector<ButtonAtlas> atlases)
    : m_atlases(std::move(atlases)) {
    for (const auto &a : m_atlases)
        m_sizes.insert({ a.width, a.height });
}

bool ButtonLabels::configure(const std::string &style, const std::map<std::string, std::string> &gamepad, int scale) {
    scale = std::clamp(scale, 1, 4);
    std::lock_guard lock(m_mutex);
    if (style == m_style && gamepad == m_gamepad && scale == m_scale)
        return false;
    const bool was_enabled = m_style == "xbox";
    m_style = style;
    m_gamepad = gamepad;
    m_scale = scale;
    // Vita3K's own bindings for a Vita button [gamepad] leaves out (input.cpp)
    static const std::map<std::string, std::string> defaults{
        { "square", "west" }, { "circle", "east" }, { "cross", "south" }, { "triangle", "north" },
        { "l", "leftshoulder" }, { "r", "rightshoulder" }
    };
    static const std::pair<ButtonSymbol, const char *> buttons[] = {
        { ButtonSymbol::Square, "square" }, { ButtonSymbol::Circle, "circle" }, { ButtonSymbol::Cross, "cross" },
        { ButtonSymbol::Triangle, "triangle" }, { ButtonSymbol::L, "l" }, { ButtonSymbol::R, "r" }
    };
    m_labels.clear();
    for (const auto &[symbol, name] : buttons) {
        const auto it = gamepad.find(name);
        std::string bound = it != gamepad.end() ? trim(it->second.substr(0, it->second.find(','))) : std::string();
        if (bound.empty())
            bound = defaults.at(name);
        const Label label = label_of(bound);
        if (!label.text.empty())
            m_labels[symbol] = label;
    }
    const bool enabled = style == "xbox";
    m_enabled = enabled;
    LOG_INFO("Button labels: {}{}", enabled ? "Xbox" : "PlayStation", enabled ? fmt::format(" (at {}x)", scale) : "");
    return enabled || was_enabled;
}

bool ButtonLabels::wants(SceGxmTextureBaseFormat format, uint32_t width, uint32_t height) const {
    return m_enabled.load(std::memory_order_relaxed) && format == SCE_GXM_TEXTURE_BASE_FORMAT_UBC3
        && m_sizes.contains({ static_cast<uint16_t>(width), static_cast<uint16_t>(height) });
}

bool ButtonLabels::make(SceGxmTextureBaseFormat format, uint32_t width, uint32_t height, const uint8_t *data, uint32_t size,
    std::vector<uint8_t> &rgba, uint32_t &out_width, uint32_t &out_height) {
    const uint32_t level_size = width * height; // UBC3: 16 bytes per 4x4 block
    if (size < level_size)
        return false;
    const uint64_t hash = fnv1a64(data, level_size);
    const auto atlas = std::find_if(m_atlases.begin(), m_atlases.end(), [&](const ButtonAtlas &a) {
        return a.hash == hash && a.width == width && a.height == height;
    });
    std::lock_guard lock(m_mutex);
    if (atlas == m_atlases.end() || m_style != "xbox") {
        if (atlas == m_atlases.end() && m_logged.insert(hash).second)
            LOG_DEBUG("Button labels: {}x{} texture {:016x} is not a button atlas", width, height, hash);
        return false;
    }

    // the atlas as the game draws it: unswizzled, decoded, scaled up (bilinear)
    std::vector<uint8_t> blocks(level_size);
    renderer::texture::resolve_z_order_compressed_texture(format, blocks.data(), data, width, height);
    std::vector<uint8_t> src(static_cast<size_t>(width) * height * 4);
    renderer::texture::decompress_compressed_texture(format, src.data(), blocks.data(), width, height);
    const uint32_t s = static_cast<uint32_t>(m_scale);
    const uint32_t w = width * s, h = height * s;
    rgba.assign(static_cast<size_t>(w) * h * 4, 0);
    for (uint32_t y = 0; y < h; y++) {
        const float fy = std::clamp((y + 0.5f) / s - 0.5f, 0.f, height - 1.f);
        const uint32_t y0 = static_cast<uint32_t>(fy), y1 = std::min(y0 + 1, height - 1);
        const float ty = fy - y0;
        for (uint32_t x = 0; x < w; x++) {
            const float fx = std::clamp((x + 0.5f) / s - 0.5f, 0.f, width - 1.f);
            const uint32_t x0 = static_cast<uint32_t>(fx), x1 = std::min(x0 + 1, width - 1);
            const float tx = fx - x0;
            for (int c = 0; c < 4; c++) {
                const auto at = [&](uint32_t px, uint32_t py) { return static_cast<float>(src[(py * width + px) * 4 + c]); };
                const float top = at(x0, y0) + (at(x1, y0) - at(x0, y0)) * tx;
                const float bottom = at(x0, y1) + (at(x1, y1) - at(x0, y1)) * tx;
                rgba[(static_cast<size_t>(y) * w + x) * 4 + c] = static_cast<uint8_t>(std::lround(top + (bottom - top) * ty));
            }
        }
    }

    int drawn = 0;
    for (const auto &slot : atlas->slots) {
        const auto label = m_labels.find(slot.symbol);
        if (label == m_labels.end())
            continue;
        // the ink around the symbol (its dark, opaque pixels, two to four pixels out): the colour the
        // symbol is painted over with
        uint32_t ink[3] = { 0, 0, 0 }, count = 0;
        for (int y = slot.y - 4; y < slot.y + slot.h + 4; y++) {
            for (int x = slot.x - 4; x < slot.x + slot.w + 4; x++) {
                const bool inside = x >= slot.x - 2 && x < slot.x + slot.w + 2 && y >= slot.y - 2 && y < slot.y + slot.h + 2;
                if (inside || x < 0 || y < 0 || x >= static_cast<int>(width) || y >= static_cast<int>(height))
                    continue;
                const uint8_t *p = &src[(static_cast<size_t>(y) * width + x) * 4];
                if (p[3] > 200 && p[0] + p[1] + p[2] < 3 * 90) {
                    for (int c = 0; c < 3; c++)
                        ink[c] += p[c];
                    count++;
                }
            }
        }
        for (auto &c : ink)
            c = count ? c / count : 0;
        // the symbol and a pixel around it (its soft edge)
        const uint32_t ex0 = (std::max(slot.x - 1, 0)) * s, ey0 = (std::max(slot.y - 1, 0)) * s;
        const uint32_t ex1 = std::min<uint32_t>(slot.x + slot.w + 1, width) * s, ey1 = std::min<uint32_t>(slot.y + slot.h + 1, height) * s;
        for (uint32_t y = ey0; y < ey1; y++)
            for (uint32_t x = ex0; x < ex1; x++)
                for (int c = 0; c < 3; c++)
                    rgba[(static_cast<size_t>(y) * w + x) * 4 + c] = static_cast<uint8_t>(ink[c]);
        // the letters, as tall as the symbol (a face symbol: as its larger side, it is about square)
        const bool letter_symbol = slot.symbol == ButtonSymbol::L || slot.symbol == ButtonSymbol::R;
        const float letter = static_cast<float>(letter_symbol ? slot.h : std::max(slot.w, slot.h)) * s;
        draw_text(rgba, w, h, (slot.x + slot.w / 2.f) * s, (slot.y + slot.h / 2.f) * s, letter, label->second);
        drawn++;
    }
    if (m_logged.insert(hash).second)
        LOG_INFO("Button labels: {} redrawn ({} of {} symbols)", atlas->name, drawn, atlas->slots.size());
    out_width = w;
    out_height = h;
    return true;
}

} // namespace game_frontend
