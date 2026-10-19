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

// VITA3K_NGS_TRACE=<folder>: a debugging aid for the mix. Every rack's levels every 2 s and the
// patches' volume matrices go to <folder>/ngs-trace.txt, and each source rack's output (port 0,
// all its voices, before the patches) to <folder>/rack-<address>-t<type>.raw as 16-bit mono at
// the system's rate. Nothing happens when the variable is unset.

#include <ngs/system.h>

#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <mutex>
#include <string>
#include <tuple>
#include <vector>

namespace ngs::trace {

inline const char *folder() {
    static const char *const f = [] {
        const char *const v = std::getenv("VITA3K_NGS_TRACE");
        return v && *v ? v : nullptr;
    }();
    return f;
}

inline bool on() {
    return folder() != nullptr;
}

struct RackStats {
    int type = 0, channels = 0, voices = 0;
    double src = 0, out0 = 0, out1 = 0, sent = 0;
    uint64_t voice_ticks = 0;
    std::vector<float> mix;
    FILE *raw = nullptr;
};

using PatchKey = std::tuple<Rack *, int, Rack *, int, int, int, int>; // matrix in thousandths

struct Tracer {
    std::mutex mutex;
    std::map<Rack *, RackStats> racks;
    std::map<PatchKey, double> patches; // energy sent through each (rack, port, dest, matrix)
    FILE *text = nullptr;
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now(), last = start;
    uint64_t ticks = 0;
};

inline Tracer &tracer() {
    static Tracer t;
    return t;
}

inline bool is_source_type(const int type) {
    return type == 11 || (type >= 12 && type <= 17);
}

inline double energy(const float *pcm, const int frames) {
    double e = 0;
    if (pcm)
        for (int k = 0; k < frames * 2; k++)
            e += double(pcm[k]) * pcm[k];
    return e;
}

inline void rack_created(Rack *rack, const int type, const int channels, const int voices) {
    Tracer &t = tracer();
    std::lock_guard<std::mutex> lock(t.mutex);
    RackStats &r = t.racks[rack];
    r.type = type;
    r.channels = channels;
    r.voices = voices;
    if (!t.text)
        t.text = std::fopen((std::string(folder()) + "/ngs-trace.txt").c_str(), "w");
    if (t.text)
        std::fprintf(t.text, "rack %p type %d channels %d voices %d\n", static_cast<void *>(rack), type, channels, voices);
    if (is_source_type(type) && !r.raw) {
        char name[64];
        std::snprintf(name, sizeof(name), "/rack-%p-t%d.raw", static_cast<void *>(rack), type);
        r.raw = std::fopen((std::string(folder()) + name).c_str(), "wb");
    }
}

// module 0's product (the decoder's output) of a source voice
inline void decoded(Voice *voice, const int frames) {
    Tracer &t = tracer();
    std::lock_guard<std::mutex> lock(t.mutex);
    auto it = t.racks.find(voice->rack);
    if (it != t.racks.end())
        it->second.src += energy(reinterpret_cast<const float *>(voice->products[0].data), frames);
}

inline void delivered(const MemState &mem, Voice *voice, const int frames) {
    Tracer &t = tracer();
    std::lock_guard<std::mutex> lock(t.mutex);
    auto it = t.racks.find(voice->rack);
    if (it == t.racks.end())
        return;
    RackStats &r = it->second;
    r.voice_ticks++;
    const float *const p0 = reinterpret_cast<const float *>(voice->products[0].data);
    const float *const p1 = reinterpret_cast<const float *>(voice->products[1].data);
    r.out0 += energy(p0, frames);
    r.out1 += energy(p1, frames);
    if (r.raw && p0) {
        r.mix.resize(static_cast<size_t>(frames), 0.0f);
        for (int k = 0; k < frames; k++)
            r.mix[k] += 0.5f * (p0[k * 2] + p0[k * 2 + 1]);
    }
    for (size_t port = 0; port < voice->patches.size() && port < 4; port++) {
        const float *const pcm = reinterpret_cast<const float *>(voice->products[port].data);
        if (!pcm)
            continue;
        for (const Ptr<Patch> &pp : voice->patches[port]) {
            const Patch *const patch = pp.get(mem);
            if (!patch || !patch->is_active())
                continue;
            const Voice *const dest = patch->dest.get(mem);
            const float(&m)[2][2] = patch->volume_matrix;
            double e = 0;
            for (int k = 0; k < frames; k++) {
                const double l = pcm[k * 2] * m[0][0] + (voice->rack->channels_per_voice == 1 ? 0 : pcm[k * 2 + 1] * m[1][0]);
                const double rr = pcm[k * 2] * m[0][1] + (voice->rack->channels_per_voice == 1 ? 0 : pcm[k * 2 + 1] * m[1][1]);
                e += l * l + rr * rr;
            }
            const PatchKey key{ voice->rack, int(port), dest ? dest->rack : nullptr, int(std::lround(m[0][0] * 1000)),
                int(std::lround(m[0][1] * 1000)), int(std::lround(m[1][0] * 1000)), int(std::lround(m[1][1] * 1000)) };
            t.patches[key] += e;
            r.sent += e;
        }
    }
}

inline double db(const double e, const double n) {
    return n > 0 && e > 0 ? 10.0 * std::log10(e / n) : -999.0;
}

inline void tick_end(const int frames) {
    Tracer &t = tracer();
    std::lock_guard<std::mutex> lock(t.mutex);
    t.ticks++;
    for (auto &[rack, r] : t.racks) {
        if (!r.raw)
            continue;
        r.mix.resize(static_cast<size_t>(frames), 0.0f);
        std::vector<int16_t> out(static_cast<size_t>(frames));
        for (int k = 0; k < frames; k++)
            out[k] = static_cast<int16_t>(std::lround(std::fmax(-1.0f, std::fmin(1.0f, r.mix[k])) * 32767.0f));
        std::fwrite(out.data(), sizeof(int16_t), out.size(), r.raw);
        std::fill(r.mix.begin(), r.mix.end(), 0.0f);
    }
    const auto now = std::chrono::steady_clock::now();
    if (now - t.last < std::chrono::seconds(2) || !t.text)
        return;
    t.last = now;
    const double secs = std::chrono::duration<double>(now - t.start).count();
    std::fprintf(t.text, "== %.1f s (tick %llu)\n", secs, static_cast<unsigned long long>(t.ticks));
    for (auto &[rack, r] : t.racks) {
        if (r.voice_ticks == 0)
            continue;
        const double n = double(r.voice_ticks) * frames * 2; // mean per sample per active voice
        std::fprintf(t.text, "  rack %p t%d ch%d: voice-ticks %llu src %.1f out0 %.1f out1 %.1f sent %.1f dB\n",
            static_cast<void *>(rack), r.type, r.channels, static_cast<unsigned long long>(r.voice_ticks),
            db(r.src, n), db(r.out0, n), db(r.out1, n), db(r.sent, n));
        r.src = r.out0 = r.out1 = r.sent = 0;
        r.voice_ticks = 0;
    }
    for (auto &[key, e] : t.patches) {
        const auto &[src, port, dest, m00, m01, m10, m11] = key;
        std::fprintf(t.text, "  patch %p:%d -> %p [%d %d; %d %d] energy %.3g\n", static_cast<void *>(src), port,
            static_cast<void *>(dest), m00, m01, m10, m11, e);
    }
    t.patches.clear();
    std::fflush(t.text);
}

} // namespace ngs::trace
