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

// the symbols that are no character: Xbox's Menu and View, the ring around a font page's letters, and
// a Switch Pro Controller's + and - (as big as Menu and View; the keycaps' + and - are smaller)
constexpr char GLYPH_MENU = '\x01', GLYPH_VIEW = '\x02', GLYPH_RING = '\x03', GLYPH_PLUS = '\x04', GLYPH_MINUS = '\x05';

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
        g[GLYPH_MENU] = { 1.f, { line(T, 0.2f, 1 - T, 0.2f), line(T, M, 1 - T, M), line(T, 0.8f, 1 - T, 0.8f) } };
        const float v0 = 0.62f, v1 = 0.38f;
        g[GLYPH_VIEW] = { 1.f, { line(T, T, v0, T), line(v0, T, v0, v1), line(T, T, T, v0), line(T, v0, v1, v0), // the back window, behind
                                   line(v1, v1, B, v1), line(B, v1, B, B), line(B, B, v1, B), line(v1, B, v1, v1) } };
        // a ring, around a font page's letters
        g[GLYPH_RING] = { 1.f, { arc(M, M, M - T, 0, 360) } };
        // Nintendo's + (START) and - (SELECT)
        const float pm = 0.86f; // their width and the +'s height
        g[GLYPH_PLUS] = { pm, { line(T, M, pm - T, M), line(pm / 2, M - pm / 2 + T, pm / 2, M + pm / 2 - T) } };
        g[GLYPH_MINUS] = { pm, { line(T, M, pm - T, M) } };

        // the rest of the letters, the digits, punctuation and arrows: the keys on a keyboard's keycaps
        // a rounded box w wide (O, 0), its ends half circles
        const auto oval = [](float w) {
            const float r = w / 2 - T;
            return std::vector<Stroke>{ arc(w / 2, T + r, r, 180, 360), line(w - T, T + r, w - T, B - r), arc(w / 2, B - r, r, 0, 180), line(T, B - r, T, T + r) };
        };
        const float rr = 0.235f; // the round letters' corners
        g['C'] = { 0.62f, { arc(0.31f, T + 0.215f, 0.215f, 180, 330), line(T, T + 0.215f, T, B - 0.215f), arc(0.31f, B - 0.215f, 0.215f, 30, 180) } };
        g['D'] = { 0.64f, { line(T, T, T, B), line(T, T, 0.31f, T), arc(0.31f, T + rr, rr, -90, 0), line(0.545f, T + rr, 0.545f, B - rr),
                              arc(0.31f, B - rr, rr, 0, 90), line(0.31f, B, T, B) } };
        g['E'] = { 0.56f, { line(T, T, T, B), line(T, T, 0.56f - T, T), line(T, M, 0.51f - T, M), line(T, B, 0.56f - T, B) } };
        g['F'] = { 0.56f, { line(T, T, T, B), line(T, T, 0.56f - T, T), line(T, M, 0.51f - T, M) } };
        g['G'] = { 0.66f, { arc(0.33f, T + rr, rr, 180, 330), line(T, T + rr, T, B - rr), arc(0.33f, B - rr, rr, 0, 180),
                              line(0.66f - T, B - rr, 0.66f - T, 0.55f), line(0.66f - T, 0.55f, 0.36f, 0.55f) } };
        g['H'] = { 0.64f, { line(T, T, T, B), line(0.64f - T, T, 0.64f - T, B), line(T, M, 0.64f - T, M) } };
        g['I'] = { 0.44f, { line(0.22f, T, 0.22f, B), line(T, T, 0.44f - T, T), line(T, B, 0.44f - T, B) } };
        g['J'] = { 0.56f, { line(0.56f - T, T, 0.56f - T, B - 0.19f), arc(0.275f, B - 0.19f, 0.19f, 0, 180) } };
        g['K'] = { 0.62f, { line(T, T, T, B), line(0.62f - T, T, T + 0.02f, 0.6f), line(0.27f, 0.47f, 0.62f - T, B) } };
        g['M'] = { 0.8f, { line(T, B, T, T), line(T, T, 0.4f, 0.62f), line(0.4f, 0.62f, 0.8f - T, T), line(0.8f - T, T, 0.8f - T, B) } };
        g['N'] = { 0.66f, { line(T, B, T, T), line(T, T, 0.66f - T, B), line(0.66f - T, B, 0.66f - T, T) } };
        g['O'] = { 0.66f, oval(0.66f) };
        g['P'] = { 0.62f, { line(T, T, T, B), line(T, T, b1, T), arc(b1, T + BOWL, BOWL, -90, 90), line(b1, M, T, M) } };
        g['Q'] = { 0.66f, oval(0.66f) };
        g['Q'].strokes.push_back(line(0.38f, 0.66f, 0.6f, B));
        g['T'] = { 0.62f, { line(T, T, 0.62f - T, T), line(0.31f, T, 0.31f, B) } };
        g['U'] = { 0.64f, { line(T, T, T, B - 0.225f), arc(0.32f, B - 0.225f, 0.225f, 0, 180), line(0.64f - T, B - 0.225f, 0.64f - T, T) } };
        g['V'] = { 0.68f, { line(T, T, 0.34f, B), line(0.34f, B, 0.68f - T, T) } };
        g['W'] = { 0.9f, { line(T, T, 0.25f, B), line(0.25f, B, 0.45f, 0.42f), line(0.45f, 0.42f, 0.65f, B), line(0.65f, B, 0.9f - T, T) } };
        g['Z'] = { 0.62f, { line(T, T, 0.62f - T, T), line(0.62f - T, T, T, B), line(T, B, 0.62f - T, B) } };
        g['0'] = { 0.6f, oval(0.6f) };
        g['1'] = { 0.46f, { line(0.3f, T, 0.3f, B), line(0.3f, T, 0.12f, 0.24f) } };
        g['2'] = { 0.6f, { arc(0.3f, T + 0.205f, 0.205f, 180, 390), line(0.477f, 0.4025f, T, B), line(T, B, 0.6f - T, B) } };
        g['3'] = { 0.6f, { arc(0.3f, T + BOWL, BOWL, 200, 450), arc(0.3f, M + BOWL, BOWL, -90, 160) } };
        g['4'] = { 0.62f, { line(0.42f, B, 0.42f, T), line(0.42f, T, T, 0.66f), line(T, 0.66f, 0.62f - T + 0.02f, 0.66f) } };
        g['5'] = { 0.645f, { line(0.6f - T, T, 0.13f, T), line(0.13f, T, 0.139f, 0.4635f), arc(0.3f, 0.655f, 0.25f, -130, 150) } };
        g['6'] = { 0.69f, { arc(0.345f, 0.655f, 0.25f, 0, 360), line(0.105f, 0.6f, 0.46f, T) } };
        g['7'] = { 0.6f, { line(T, T, 0.6f - T, T), line(0.6f - T, T, 0.24f, B) } };
        g['8'] = { 0.6f, { arc(0.3f, 0.285f, 0.19f, 0, 360), arc(0.3f, 0.69f, 0.215f, 0, 360) } };
        g['9'] = { 0.69f, { arc(0.345f, 0.345f, 0.25f, 0, 360), line(0.585f, 0.4f, 0.23f, B) } };
        g[','] = { 0.3f, { line(0.16f, 0.82f, 0.1f, 0.98f) } };
        g['.'] = { 0.26f, { arc(0.13f, B - 0.01f, 0.01f, 0, 360) } };
        g[';'] = { 0.3f, { arc(0.15f, 0.4f, 0.01f, 0, 360), line(0.16f, 0.82f, 0.1f, 0.98f) } };
        g['/'] = { 0.5f, { line(T, B, 0.5f - T, T) } };
        g['\\'] = { 0.5f, { line(T, T, 0.5f - T, B) } };
        g['\''] = { 0.26f, { line(0.13f, T, 0.13f, 0.32f) } };
        g['`'] = { 0.3f, { line(0.1f, T, 0.2f, 0.28f) } };
        g['['] = { 0.36f, { line(0.3f, T, T, T), line(T, T, T, B), line(T, B, 0.3f, B) } };
        g[']'] = { 0.36f, { line(0.06f, T, 0.265f, T), line(0.265f, T, 0.265f, B), line(0.265f, B, 0.06f, B) } };
        g['-'] = { 0.5f, { line(T, M, 0.5f - T, M) } };
        g['='] = { 0.56f, { line(T, 0.37f, 0.56f - T, 0.37f), line(T, 0.63f, 0.56f - T, 0.63f) } };
        g['+'] = { 0.6f, { line(T, M, 0.6f - T, M), line(0.3f, 0.25f, 0.3f, 0.75f) } };
        g['*'] = { 0.56f, { line(0.28f, 0.22f, 0.28f, 0.78f), line(0.14f, 0.36f, 0.42f, 0.64f), line(0.42f, 0.36f, 0.14f, 0.64f) } };
        g[KEYCAP_UP] = { 0.62f, { line(0.31f, T, 0.31f, B), line(0.31f, T, T, 0.4f), line(0.31f, T, 0.62f - T, 0.4f) } };
        g[KEYCAP_DOWN] = { 0.62f, { line(0.31f, T, 0.31f, B), line(0.31f, B, T, 0.6f), line(0.31f, B, 0.62f - T, 0.6f) } };
        g[KEYCAP_LEFT] = { 0.86f, { line(T, M, 0.86f - T, M), line(T, M, 0.4f, 0.2f), line(T, M, 0.4f, 0.8f) } };
        g[KEYCAP_RIGHT] = { 0.86f, { line(T, M, 0.86f - T, M), line(0.86f - T, M, 0.46f, 0.2f), line(0.86f - T, M, 0.46f, 0.8f) } };
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

// a keycap: a rounded rectangle's outline centred on (cx, cy), w x h pixels along the outline's middle,
// `stroke` pixels wide, in (r, g, b)
void draw_keycap(Image &img, float cx, float cy, float w, float h, float stroke, uint8_t r, uint8_t g, uint8_t b) {
    const float radius = std::min(w, h) * 0.28f;
    const float hx = w / 2 - radius, hy = h / 2 - radius;
    const int x0 = std::max(0, static_cast<int>(std::floor(cx - w / 2 - stroke)) - 1);
    const int x1 = std::min<int>(img.w, static_cast<int>(std::ceil(cx + w / 2 + stroke)) + 1);
    const int y0 = std::max(0, static_cast<int>(std::floor(cy - h / 2 - stroke)) - 1);
    const int y1 = std::min<int>(img.h, static_cast<int>(std::ceil(cy + h / 2 + stroke)) + 1);
    for (int py = y0; py < y1; py++) {
        for (int px = x0; px < x1; px++) {
            // the distance to the rounded rectangle's edge
            const float qx = std::abs(px + 0.5f - cx) - hx, qy = std::abs(py + 0.5f - cy) - hy;
            const float edge = std::hypot(std::max(qx, 0.f), std::max(qy, 0.f)) + std::min(std::max(qx, qy), 0.f) - radius;
            const float cover = std::clamp(stroke / 2 - std::abs(edge) + 0.5f, 0.f, 1.f);
            if (cover <= 0)
                continue;
            uint8_t *p = img.at(px, py);
            p[0] = static_cast<uint8_t>(std::lround(p[0] + (r - p[0]) * cover));
            p[1] = static_cast<uint8_t>(std::lround(p[1] + (g - p[1]) * cover));
            p[2] = static_cast<uint8_t>(std::lround(p[2] + (b - p[2]) * cover));
            p[3] = std::max<uint8_t>(p[3], static_cast<uint8_t>(std::lround(cover * 255)));
        }
    }
}

// A key's name in a keycap: the keycap `box_h` pixels tall (its outline's middle), as wide as the name
// needs but at least square and at most `max_w`; the name about half as tall, smaller and then narrower
// when it doesn't fit. Returns the keycap's width.
float keycap_width(const std::string &text, float box_h, float max_w, float stroke, float &text_h, float &xscale) {
    const float pad = stroke + box_h * 0.14f; // between the outline and the name
    const float units = text_width(text);
    text_h = box_h * 0.54f;
    const float want = std::max(box_h, units * text_h + 2 * pad);
    const float w = std::min(want, std::max(max_w, box_h * 0.6f));
    const float room = w - 2 * pad;
    if (units * text_h > room)
        text_h = std::max(box_h * 0.4f, room / units);
    xscale = std::clamp(room / (units * text_h), 0.3f, 1.f);
    return w;
}

std::string lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
    return s;
}

// the face buttons' letters by position on a Nintendo pad (A east, B south, X north, Y west), and on
// the Android handhelds lettered the same way (Retroid Pocket, AYN, ...)
const std::map<std::string, std::string> &nintendo_face() {
    static const std::map<std::string, std::string> face{ { "south", "B" }, { "east", "A" }, { "west", "Y" }, { "north", "X" } };
    return face;
}

// the styles that redraw the game's icons (playstation keeps them)
bool redraws(const std::string &style) {
    return style == "xbox" || style == "nintendo" || style == "handheld" || style == "keyboard";
}

// the label a gamepad button ([gamepad]'s names, SDL's) shows in `style` (xbox, nintendo, handheld);
// empty text: none. face: the face buttons' letters by position where they differ from an Xbox pad's
ButtonLabels::Label label_of(const std::string &name, const std::map<std::string, std::string> &face, const std::string &style) {
    std::string button = lower(name);
    static const std::map<std::string, std::string> aliases{ { "a", "south" }, { "b", "east" }, { "x", "west" }, { "y", "north" } };
    if (const auto it = aliases.find(button); it != aliases.end())
        button = it->second;
    static const std::map<std::string, std::string> xbox_face{ { "south", "A" }, { "east", "B" }, { "west", "X" }, { "north", "Y" } };
    if (const auto it = xbox_face.find(button); it != xbox_face.end()) {
        const auto f = face.find(button);
        const std::string letter = f != face.end() ? f->second : it->second;
        if (letter != "A" && letter != "B" && letter != "X" && letter != "Y")
            return {};
        // Nintendo's and the handhelds' letters are plain: Xbox's colours would put a green A where
        // an Xbox pad has B
        if (style != "xbox")
            return { letter, 240, 240, 240 };
        // in the colours of the letters on an Xbox pad
        static const std::map<std::string, ButtonLabels::Label> colours{
            { "A", { "A", 110, 200, 70 } }, { "B", { "B", 235, 75, 60 } }, { "X", { "X", 70, 150, 240 } }, { "Y", { "Y", 245, 200, 50 } }
        };
        return colours.at(letter);
    }
    // Xbox's, a Switch Pro Controller's (L, R, + and -), or the handhelds' (L1, R1, Xbox's Menu and View)
    static const std::map<std::string, std::string> xbox_others{
        { "leftshoulder", "LB" }, { "rightshoulder", "RB" }, { "lefttrigger", "LT" }, { "righttrigger", "RT" }, { "leftstick", "LS" }, { "rightstick", "RS" },
        { "start", std::string(1, GLYPH_MENU) }, { "back", std::string(1, GLYPH_VIEW) }
    };
    static const std::map<std::string, std::string> nintendo_others{
        { "leftshoulder", "L" }, { "rightshoulder", "R" }, { "lefttrigger", "ZL" }, { "righttrigger", "ZR" }, { "leftstick", "LS" }, { "rightstick", "RS" },
        { "start", std::string(1, GLYPH_PLUS) }, { "back", std::string(1, GLYPH_MINUS) }
    };
    // the handhelds print L1 and R1 on their shoulder buttons (Retroid Pocket, AYN)
    static const std::map<std::string, std::string> handheld_others{
        { "leftshoulder", "L1" }, { "rightshoulder", "R1" }, { "lefttrigger", "L2" }, { "righttrigger", "R2" }, { "leftstick", "LS" }, { "rightstick", "RS" },
        { "start", std::string(1, GLYPH_MENU) }, { "back", std::string(1, GLYPH_VIEW) }
    };
    const auto &others = style == "nintendo" ? nintendo_others : style == "handheld" ? handheld_others : xbox_others;
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
    std::vector<std::string> choices{ "playstation", "xbox", "nintendo" };
    std::vector<std::string> labels{ "PlayStation", "Xbox", "Nintendo (Switch Pro)" };
#ifdef __ANDROID__
    // the Android handhelds' built-in controls (Retroid Pocket, AYN, ...): only offered on Android
    choices.push_back("handheld");
    labels.push_back("Android handheld");
    const char *const handheld = "handheld (an Android handheld's built-in controls, such as a Retroid\n"
                                 "Pocket's or an AYN's: Nintendo's letters, L1, R1, and Xbox's Menu and View), ";
#else
    const char *const handheld = "";
#endif
    choices.insert(choices.end(), { "keyboard", "auto" });
    labels.insert(labels.end(), { "Keyboard", "As the device used" });
    SettingSpec s{ "button_labels", SettingSpec::Kind::Choice, "playstation", 0, 0, choices,
        fmt::format("The buttons the game's prompts show: playstation (the game's own icons), xbox (A, B, X, Y, LB,\n"
                    "RB, and Xbox's Menu and View for START and SELECT: the gamepad button that [gamepad] binds to\n"
                    "each Vita button), nintendo (a Switch Pro Controller's: A east, B south, X north, Y west, L, R,\n"
                    "+ and -), {}keyboard (the key [keyboard] binds to each, the first of two, in a keycap:\n"
                    "X, C, Z, V, Q, E, ENT, SHF ...), or auto (the device used last: keyboard after a key or a mouse\n"
                    "button, and for a gamepad xbox while it has lettered buttons, with its own letters, nintendo\n"
                    "for a Nintendo pad, playstation for a PlayStation pad; before any input, the first gamepad's,\n"
                    "or keyboard without one). Also in the tutorial pictures and the text's few button symbols.\n"
                    "The D-pad, the sticks and TOUCH stay.",
            handheld) };
    s.live = true;
    s.title = "Button prompts";
    s.choice_labels = labels;
    return s;
}

std::string auto_button_style(const CtrlState &ctrl, std::map<std::string, std::string> &face, InputDevice last) {
    face.clear();
    if (last == InputDevice::Keyboard)
        return "keyboard";
    const Controller *first = nullptr;
    for (const auto &[_, c] : ctrl.controllers)
        if (c.controller && (!first || c.port < first->port))
            first = &c;
    if (!first) {
#ifdef __ANDROID__
        return "playstation"; // the on-screen controls, PlayStation's symbols
#else
        return "keyboard";
#endif
    }
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
    // a Switch Pro Controller or another pad lettered as Nintendo's: its own style (L, R, + and -)
    if (face == nintendo_face())
        return "nintendo";
    return "xbox";
}

ButtonLabels::ButtonLabels(std::vector<ButtonAtlas> atlases)
    : m_atlases(std::move(atlases)) {
    for (const auto &a : m_atlases)
        m_kinds.insert({ a.format, a.width, a.height });
}

bool ButtonLabels::configure(const std::string &style, const std::map<std::string, std::string> &gamepad, int scale,
    const std::map<std::string, std::string> &face, const std::map<std::string, std::string> &keyboard) {
    scale = std::clamp(scale, 1, 4);
    std::lock_guard lock(m_mutex);
    if (style == m_style && gamepad == m_gamepad && scale == m_scale && face == m_face && keyboard == m_keyboard)
        return false;
    const bool was_enabled = redraws(m_style);
    m_style = style;
    m_gamepad = gamepad;
    m_scale = scale;
    m_face = face;
    m_keyboard = keyboard;
    // Vita3K's own bindings for a Vita button [gamepad] leaves out (input.cpp)
    static const std::pair<ButtonSymbol, std::pair<const char *, const char *>> buttons[] = {
        { ButtonSymbol::Square, { "square", "west" } }, { ButtonSymbol::Circle, { "circle", "east" } },
        { ButtonSymbol::Cross, { "cross", "south" } }, { ButtonSymbol::Triangle, { "triangle", "north" } },
        { ButtonSymbol::L, { "l", "leftshoulder" } }, { ButtonSymbol::R, { "r", "rightshoulder" } },
        { ButtonSymbol::Start, { "start", "start" } }, { ButtonSymbol::Select, { "select", "back" } }
    };
    m_labels.clear();
    std::string keys; // for the log
    for (const auto &[symbol, names] : buttons) {
        if (style == "keyboard") {
            // the first key [keyboard] binds to it (its default when the file has no line for it; none
            // when the line is empty: the PlayStation symbol stays)
            const auto binding = std::find_if(button_bindings().begin(), button_bindings().end(),
                [&](const ButtonBinding &b) { return names.first == std::string(b.name); });
            const std::string fallback = binding != button_bindings().end() ? binding->keys : "";
            const auto it = keyboard.find(names.first);
            std::vector<std::string> warnings;
            auto codes = parse_keys(it != keyboard.end() ? it->second : fallback, names.first, warnings);
            if (codes.empty() && it != keyboard.end() && !split_bindings(it->second).empty())
                codes = parse_keys(fallback, names.first, warnings);
            if (codes.empty())
                continue;
            Label label{ key_cap_text(codes[0]), 240, 240, 240 };
            label.keycap = true;
            keys += (keys.empty() ? "" : " ") + key_name(codes[0]);
            m_labels[symbol] = label;
            continue;
        }
        // the gamepad button [gamepad] binds to it (its default when the file has no line for it; none
        // when the line is empty)
        const auto it = gamepad.find(names.first);
        std::string bound = it != gamepad.end() ? trim(it->second.substr(0, it->second.find(','))) : std::string(names.second);
        // Nintendo's and the handhelds' face buttons are lettered by their own positions
        const Label label = label_of(bound, style == "nintendo" || style == "handheld" ? nintendo_face() : face, style);
        if (!label.text.empty())
            m_labels[symbol] = label;
    }
    const bool enabled = redraws(style);
    m_enabled = enabled;
    std::string letters;
    for (const auto &[position, letter] : face)
        letters += letter;
    if (style == "keyboard")
        LOG_INFO("Button labels: keyboard (at {}x, keys {})", scale, keys);
    else if (style == "nintendo" || style == "handheld")
        LOG_INFO("Button labels: {} (at {}x)", style == "nintendo" ? "Nintendo" : "Android handheld", scale);
    else
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
    if (atlas == m_atlases.end() || !redraws(m_style)) {
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
    // a picture kept upside down: painted upright, and turned back below
    const auto flip_rows = [](std::vector<uint8_t> &pixels, uint32_t w, uint32_t h) {
        for (uint32_t y = 0; y < h / 2; y++)
            std::swap_ranges(pixels.begin() + static_cast<size_t>(y) * w * 4, pixels.begin() + static_cast<size_t>(y + 1) * w * 4,
                pixels.begin() + static_cast<size_t>(h - 1 - y) * w * 4);
    };
    if (atlas->flip_y)
        flip_rows(src, width, height);
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
            if (label->second.keycap) {
                // a keycap with the key's name in it, as wide as the name needs within the cell
                const float stroke = std::max(1.4f * s, ring * 0.07f);
                const float box_h = ring - stroke;
                float text_h = 0, xscale = 1;
                const float box_w = keycap_width(label->second.text, box_h, slot.w * 0.96f * s - stroke, stroke, text_h, xscale);
                draw_keycap(img, cx, cy, box_w, box_h, stroke, 255, 255, 255);
                draw_text(img, cx, cy, text_h, xscale, std::max(0.17f, 1.3f * s / text_h), label->second.text, 255, 255, 255);
                drawn++;
                continue;
            }
            draw_text(img, cx, cy, ring, 1, std::max(0.07f, 1.4f * s / ring), std::string(1, GLYPH_RING), 255, 255, 255);
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
        const ButtonSymbol sym = slot.symbol;
        const bool word = sym == ButtonSymbol::Start || sym == ButtonSymbol::Select;
        const Label &l = label->second;
        if (l.keycap) {
            // a keycap with the key's name in it, in the symbol's place: as tall as the symbol (START and
            // SELECT: half again as tall as the word, as wide as the name needs within it), a little
            // taller than wide at most
            const float cx = (slot.x + slot.w / 2.f) * s, cy = (slot.y + slot.h / 2.f) * s;
            const float stroke = std::max(0.9f * s, slot.h * s * 0.08f);
            const float box_h = (word ? slot.h * 1.5f : std::min<float>(slot.h, slot.w * 1.15f)) * s - stroke;
            float text_h = 0, xscale = 1;
            const float box_w = keycap_width(l.text, box_h, slot.w * s - stroke, stroke, text_h, xscale);
            // the ink under a keycap that reaches past the word
            if (word) {
                const uint32_t kx0 = static_cast<uint32_t>(std::max(0.f, cx - box_w / 2 - stroke));
                const uint32_t kx1 = static_cast<uint32_t>(std::min<float>(w, cx + box_w / 2 + stroke));
                const uint32_t ky0 = static_cast<uint32_t>(std::max(0.f, cy - box_h / 2 - stroke));
                const uint32_t ky1 = static_cast<uint32_t>(std::min<float>(h, cy + box_h / 2 + stroke));
                for (uint32_t y = ky0; y < ky1; y++)
                    for (uint32_t x = kx0; x < kx1; x++)
                        for (int c = 0; c < 3; c++)
                            img.at(x, y)[c] = static_cast<uint8_t>(ink[c]);
            }
            const uint8_t v = slot.white ? 255 : l.r;
            draw_keycap(img, cx, cy, box_w, box_h, stroke, v, slot.white ? 255 : l.g, slot.white ? 255 : l.b);
            draw_text(img, cx, cy, text_h, xscale, std::max(0.17f, 0.9f * s / text_h), l.text, v, slot.white ? 255 : l.g, slot.white ? 255 : l.b);
            drawn++;
            continue;
        }
        // the letters, as tall as the symbol (START and SELECT: their Xbox symbol, half again as tall
        // as the word), as narrow as a symbol drawn squeezed
        const float aspect = static_cast<float>(slot.w) / slot.h, natural = natural_aspect(sym);
        const bool face = sym != ButtonSymbol::L && sym != ButtonSymbol::R && !word;
        const float tall = word ? slot.h * 1.5f : (face && aspect >= natural ? std::max(slot.w, slot.h) : slot.h);
        const float xscale = word ? 1.f : std::clamp(aspect / natural, 0.4f, 1.f);
        if (slot.white)
            draw_text(img, (slot.x + slot.w / 2.f) * s, (slot.y + slot.h / 2.f) * s, tall * s, xscale, STROKE, l.text, 255, 255, 255);
        else
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
    if (atlas->flip_y)
        flip_rows(rgba, w, h);
    if (m_logged.insert(hash).second)
        LOG_INFO("Button labels: {} redrawn ({} of {} symbols)", atlas->name, drawn, atlas->slots.size());
    out_width = w;
    out_height = h;
    return true;
}

} // namespace game_frontend
