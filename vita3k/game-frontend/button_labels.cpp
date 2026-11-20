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

#include <ctrl/state.h>
#include <gxm/types.h>
#include <renderer/functions.h>
#include <util/log.h>

#include <SDL3/SDL_gamepad.h>

#include <algorithm>
#include <cctype>
#include <cmath>

namespace game_frontend {

namespace {

// The letters and symbols, drawn as strokes of one width (no font): x right, y down, in units of the
// glyph's height; arcs by their centre, radius and angles in degrees (0 right, 90 down). Like the
// game's own symbols, which are light strokes in an ink blot.
struct Stroke {
    bool arc;
    float a, b, c, d, e; // line: (a, b)-(c, d); arc: centre (a, b), radius c, angles d..e
};
struct Glyph {
    float width;
    std::vector<Stroke> strokes;
};

constexpr float STROKE = 0.19f; // the letters' strokes' width
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
        // Xbox's Menu button (three lines) and View button (two overlapping windows)
        g['='] = { 1.f, { line(T, 0.2f, 1 - T, 0.2f), line(T, M, 1 - T, M), line(T, 0.8f, 1 - T, 0.8f) } };
        const float v0 = 0.62f, v1 = 0.38f;
        g['%'] = { 1.f, { line(T, T, v0, T), line(v0, T, v0, v1), line(T, T, T, v0), line(T, v0, v1, v0), // the back window, behind
                            line(v1, v1, B, v1), line(B, v1, B, B), line(B, B, v1, B), line(v1, B, v1, v1) } };
        // a ring, around a font page's letters
        g['o'] = { 1.f, { arc(M, M, M - T, 0, 360) } };
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

struct Image {
    std::vector<uint8_t> &px; // RGBA
    uint32_t w, h;
    uint8_t *at(uint32_t x, uint32_t y) { return &px[(static_cast<size_t>(y) * w + x) * 4]; }
};

// draws `text` centred on (cx, cy), `height` pixels tall and `xscale` times as wide as drawn upright,
// with strokes `stroke` of the height wide, in (r, g, b)
void draw_text(Image &img, float cx, float cy, float height, float xscale, float stroke, const std::string &text, uint8_t r, uint8_t g, uint8_t b) {
    const float hx = height * xscale; // a glyph unit across, in pixels
    float x = cx - text_width(text) * hx / 2;
    const float top = cy - height / 2;
    for (const char c : text) {
        const Glyph *gl = glyph(c);
        if (!gl)
            continue;
        const int x0 = std::max(0, static_cast<int>(std::floor(x)) - 1);
        const int x1 = std::min<int>(img.w, static_cast<int>(std::ceil(x + gl->width * hx)) + 1);
        const int y0 = std::max(0, static_cast<int>(std::floor(top)) - 1);
        const int y1 = std::min<int>(img.h, static_cast<int>(std::ceil(top + height)) + 1);
        for (int py = y0; py < y1; py++) {
            for (int px = x0; px < x1; px++) {
                // the pixel's centre in the glyph's units
                const float u = (px + 0.5f - x) / hx, v = (py + 0.5f - top) / height;
                float d = 1e9f;
                for (const auto &s : gl->strokes)
                    d = std::min(d, stroke_distance(s, u, v));
                const float cover = std::clamp((stroke / 2 - d) * std::min(height, hx) + 0.5f, 0.f, 1.f);
                if (cover <= 0)
                    continue;
                uint8_t *p = img.at(px, py);
                p[0] = static_cast<uint8_t>(std::lround(p[0] + (r - p[0]) * cover));
                p[1] = static_cast<uint8_t>(std::lround(p[1] + (g - p[1]) * cover));
                p[2] = static_cast<uint8_t>(std::lround(p[2] + (b - p[2]) * cover));
                p[3] = std::max<uint8_t>(p[3], static_cast<uint8_t>(std::lround(cover * 255)));
            }
        }
        x += (gl->width + GAP) * hx;
    }
}

std::string lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
    return s;
}

// the label a gamepad button ([gamepad]'s names, SDL's) shows; empty text: none. face: the face
// buttons' letters by position where they differ from an Xbox pad's
ButtonLabels::Label label_of(const std::string &name, const std::map<std::string, std::string> &face) {
    std::string button = lower(name);
    static const std::map<std::string, std::string> aliases{ { "a", "south" }, { "b", "east" }, { "x", "west" }, { "y", "north" } };
    if (const auto it = aliases.find(button); it != aliases.end())
        button = it->second;
    static const std::map<std::string, std::string> xbox_face{ { "south", "A" }, { "east", "B" }, { "west", "X" }, { "north", "Y" } };
    if (const auto it = xbox_face.find(button); it != xbox_face.end()) {
        const auto f = face.find(button);
        const std::string letter = f != face.end() ? f->second : it->second;
        // in the colours of the letters on an Xbox pad
        static const std::map<std::string, ButtonLabels::Label> colours{
            { "A", { "A", 110, 200, 70 } }, { "B", { "B", 235, 75, 60 } }, { "X", { "X", 70, 150, 240 } }, { "Y", { "Y", 245, 200, 50 } }
        };
        const auto c = colours.find(letter);
        return c != colours.end() ? c->second : ButtonLabels::Label{};
    }
    static const std::map<std::string, std::string> others{
        { "leftshoulder", "LB" }, { "rightshoulder", "RB" }, { "leftstick", "LS" }, { "rightstick", "RS" },
        { "start", "=" }, { "back", "%" }
    };
    if (const auto it = others.find(button); it != others.end())
        return { it->second, 240, 240, 240 };
    return {};
}

std::string trim(const std::string &s) {
    const auto b = s.find_first_not_of(" \t");
    const auto e = s.find_last_not_of(" \t");
    return b == std::string::npos ? std::string() : s.substr(b, e - b + 1);
}

// a symbol's usual width / height: a box narrower than that is a symbol drawn squeezed
float natural_aspect(ButtonSymbol s) {
    switch (s) {
    case ButtonSymbol::Triangle: return 1.15f;
    case ButtonSymbol::L: return 0.67f;
    case ButtonSymbol::R: return 0.75f;
    default: return 1.f;
    }
}

uint32_t level_size(uint32_t format, uint32_t width, uint32_t height) {
    switch (format) {
    case SCE_GXM_TEXTURE_BASE_FORMAT_UBC1: return width * height / 2; // 8 bytes per 4x4 block
    case SCE_GXM_TEXTURE_BASE_FORMAT_UBC3: return width * height; // 16 bytes per 4x4 block
    case SCE_GXM_TEXTURE_BASE_FORMAT_U8: return width * height;
    default: return 0;
    }
}

} // namespace

uint64_t fnv1a64(const uint8_t *data, size_t size) {
    uint64_t h = 0xcbf29ce484222325ull;
    for (size_t i = 0; i < size; i++)
        h = (h ^ data[i]) * 0x100000001b3ull;
    return h;
}

SettingSpec button_labels_setting() {
    SettingSpec s{ "button_labels", SettingSpec::Kind::Choice, "playstation", 0, 0, { "playstation", "xbox", "auto" },
        "The buttons the game's prompts show: playstation (the game's own icons), xbox (A, B, X, Y, LB,\n"
        "RB, and Xbox's Menu and View for START and SELECT: the gamepad button that [gamepad] binds to\n"
        "each Vita button), or auto (xbox while the first gamepad has lettered buttons, with its own\n"
        "letters; playstation for a PlayStation pad or none). Also in the tutorial pictures and the\n"
        "text's few button symbols. The D-pad, the sticks and TOUCH are the same on both." };
    s.live = true;
    s.title = "Button prompts";
    s.choice_labels = { "PlayStation", "Xbox", "As the gamepad" };
    return s;
}

std::string auto_button_style(const CtrlState &ctrl, std::map<std::string, std::string> &face) {
    face.clear();
    const Controller *first = nullptr;
    for (const auto &[_, c] : ctrl.controllers)
        if (c.controller && (!first || c.port < first->port))
            first = &c;
    if (!first)
        return "playstation";
    static const std::pair<const char *, SDL_GamepadButton> positions[] = {
        { "south", SDL_GAMEPAD_BUTTON_SOUTH }, { "east", SDL_GAMEPAD_BUTTON_EAST },
        { "west", SDL_GAMEPAD_BUTTON_WEST }, { "north", SDL_GAMEPAD_BUTTON_NORTH }
    };
    for (const auto &[position, button] : positions) {
        switch (SDL_GetGamepadButtonLabel(first->controller.get(), button)) {
        case SDL_GAMEPAD_BUTTON_LABEL_A: face[position] = "A"; break;
        case SDL_GAMEPAD_BUTTON_LABEL_B: face[position] = "B"; break;
        case SDL_GAMEPAD_BUTTON_LABEL_X: face[position] = "X"; break;
        case SDL_GAMEPAD_BUTTON_LABEL_Y: face[position] = "Y"; break;
        case SDL_GAMEPAD_BUTTON_LABEL_CROSS:
        case SDL_GAMEPAD_BUTTON_LABEL_CIRCLE:
        case SDL_GAMEPAD_BUTTON_LABEL_SQUARE:
        case SDL_GAMEPAD_BUTTON_LABEL_TRIANGLE:
            face.clear();
            return "playstation";
        default:
            break;
        }
    }
    return "xbox";
}

ButtonLabels::ButtonLabels(std::vector<ButtonAtlas> atlases)
    : m_atlases(std::move(atlases)) {
    for (const auto &a : m_atlases)
        m_kinds.insert({ a.format, a.width, a.height });
}

bool ButtonLabels::configure(const std::string &style, const std::map<std::string, std::string> &gamepad, int scale,
    const std::map<std::string, std::string> &face) {
    scale = std::clamp(scale, 1, 4);
    std::lock_guard lock(m_mutex);
    if (style == m_style && gamepad == m_gamepad && scale == m_scale && face == m_face)
        return false;
    const bool was_enabled = m_style == "xbox";
    m_style = style;
    m_gamepad = gamepad;
    m_scale = scale;
    m_face = face;
    // Vita3K's own bindings for a Vita button [gamepad] leaves out (input.cpp)
    static const std::pair<ButtonSymbol, std::pair<const char *, const char *>> buttons[] = {
        { ButtonSymbol::Square, { "square", "west" } }, { ButtonSymbol::Circle, { "circle", "east" } },
        { ButtonSymbol::Cross, { "cross", "south" } }, { ButtonSymbol::Triangle, { "triangle", "north" } },
        { ButtonSymbol::L, { "l", "leftshoulder" } }, { ButtonSymbol::R, { "r", "rightshoulder" } },
        { ButtonSymbol::Start, { "start", "start" } }, { ButtonSymbol::Select, { "select", "back" } }
    };
    m_labels.clear();
    for (const auto &[symbol, names] : buttons) {
        const auto it = gamepad.find(names.first);
        std::string bound = it != gamepad.end() ? trim(it->second.substr(0, it->second.find(','))) : std::string();
        if (bound.empty())
            bound = names.second;
        const Label label = label_of(bound, face);
        if (!label.text.empty())
            m_labels[symbol] = label;
    }
    const bool enabled = style == "xbox";
    m_enabled = enabled;
    std::string letters;
    for (const auto &[position, letter] : face)
        letters += letter;
    LOG_INFO("Button labels: {}{}", enabled ? "Xbox" : "PlayStation",
        enabled ? fmt::format(" (at {}x{})", scale, letters.empty() ? "" : ", face buttons " + letters) : "");
    return enabled || was_enabled;
}

bool ButtonLabels::wants(SceGxmTextureBaseFormat format, uint32_t width, uint32_t height) const {
    return m_enabled.load(std::memory_order_relaxed)
        && m_kinds.contains({ static_cast<uint32_t>(format), static_cast<uint16_t>(width), static_cast<uint16_t>(height) });
}

bool ButtonLabels::make(SceGxmTextureBaseFormat format, uint32_t width, uint32_t height, const uint8_t *data, uint32_t size,
    std::vector<uint8_t> &rgba, uint32_t &out_width, uint32_t &out_height) {
    const uint32_t bytes = level_size(format, width, height);
    if (!bytes || size < bytes)
        return false;
    const uint64_t hash = fnv1a64(data, bytes);
    const auto atlas = std::find_if(m_atlases.begin(), m_atlases.end(), [&](const ButtonAtlas &a) {
        return a.hash == hash && a.format == format && a.width == width && a.height == height;
    });
    std::lock_guard lock(m_mutex);
    if (atlas == m_atlases.end() || m_style != "xbox") {
        if (atlas == m_atlases.end() && m_logged.insert(hash).second)
            LOG_DEBUG("Button labels: {}x{} texture {:016x} has no buttons we know", width, height, hash);
        return false;
    }
    const bool font = format == SCE_GXM_TEXTURE_BASE_FORMAT_U8;

    // the texture as the game draws it: unswizzled, decoded (a font page: white, its value as alpha),
    // scaled up (bilinear)
    std::vector<uint8_t> src(static_cast<size_t>(width) * height * 4);
    if (font) {
        std::vector<uint8_t> linear(bytes);
        renderer::texture::swizzled_texture_to_linear_texture(linear.data(), data, width, height, 8);
        for (size_t i = 0; i < linear.size(); i++) {
            src[i * 4] = src[i * 4 + 1] = src[i * 4 + 2] = 255;
            src[i * 4 + 3] = linear[i];
        }
    } else {
        std::vector<uint8_t> blocks(bytes);
        renderer::texture::resolve_z_order_compressed_texture(format, blocks.data(), data, width, height);
        renderer::texture::decompress_compressed_texture(format, src.data(), blocks.data(), width, height);
    }
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
    Image img{ rgba, w, h };

    int drawn = 0;
    for (const auto &slot : atlas->slots) {
        const auto label = m_labels.find(slot.symbol);
        if (label == m_labels.end())
            continue;
        if (font) {
            // the glyph's cell, emptied; a ring with the letter in it, white (the text's colour comes
            // from the game)
            for (uint32_t y = slot.y * s; y < std::min<uint32_t>(slot.y + slot.h, height) * s; y++)
                for (uint32_t x = slot.x * s; x < std::min<uint32_t>(slot.x + slot.w, width) * s; x++)
                    img.at(x, y)[3] = 0;
            const float ring = std::min(slot.w, slot.h) * 0.96f * s;
            const float cx = (slot.x + slot.w / 2.f) * s, cy = (slot.y + slot.h / 2.f) * s;
            draw_text(img, cx, cy, ring, 1, std::max(0.07f, 1.4f * s / ring), "o", 255, 255, 255);
            const float letters = ring * 0.48f;
            draw_text(img, cx, cy, letters, label->second.text.size() > 1 ? 0.8f : 1.f, std::max(STROKE, 1.4f * s / letters),
                label->second.text, 255, 255, 255);
            drawn++;
            continue;
        }
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
        const uint32_t ex0 = std::max(slot.x - 1, 0) * s, ey0 = std::max(slot.y - 1, 0) * s;
        const uint32_t ex1 = std::min<uint32_t>(slot.x + slot.w + 1, width) * s, ey1 = std::min<uint32_t>(slot.y + slot.h + 1, height) * s;
        for (uint32_t y = ey0; y < ey1; y++)
            for (uint32_t x = ex0; x < ex1; x++)
                for (int c = 0; c < 3; c++)
                    img.at(x, y)[c] = static_cast<uint8_t>(ink[c]);
        // the letters, as tall as the symbol (START and SELECT: their Xbox symbol, half again as tall
        // as the word), as narrow as a symbol drawn squeezed
        const ButtonSymbol sym = slot.symbol;
        const bool word = sym == ButtonSymbol::Start || sym == ButtonSymbol::Select;
        const float aspect = static_cast<float>(slot.w) / slot.h, natural = natural_aspect(sym);
        const bool face = sym != ButtonSymbol::L && sym != ButtonSymbol::R && !word;
        const float tall = word ? slot.h * 1.5f : (face && aspect >= natural ? std::max(slot.w, slot.h) : slot.h);
        const float xscale = word ? 1.f : std::clamp(aspect / natural, 0.4f, 1.f);
        const Label &l = label->second;
        draw_text(img, (slot.x + slot.w / 2.f) * s, (slot.y + slot.h / 2.f) * s, tall * s, xscale, STROKE, l.text, l.r, l.g, l.b);
        drawn++;
    }
    if (font) {
        // back to the font page's layout: its value in R, G and B (the 1RRR swizzle), opaque
        for (size_t i = 0; i < rgba.size(); i += 4) {
            rgba[i] = rgba[i + 1] = rgba[i + 2] = rgba[i + 3];
            rgba[i + 3] = 255;
        }
    }
    if (m_logged.insert(hash).second)
        LOG_INFO("Button labels: {} redrawn ({} of {} symbols)", atlas->name, drawn, atlas->slots.size());
    out_width = w;
    out_height = h;
    return true;
}

} // namespace game_frontend
