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

#include <atomic>
#include <chrono>
#include <cstdint>

// VITA3K_FRAME_LOG=<file>: one line per frame the renderer starts, with where its time went (debugging aid for frame
// drops): see renderer::vulkan::new_frame
namespace renderer::frame_stats {

inline std::atomic<uint64_t> pipeline_wait_us{ 0 };
inline std::atomic<uint64_t> upload_us{ 0 };
inline std::atomic<uint32_t> uploads{ 0 };

// adds the time from its creation to its end to a counter
struct Timer {
    std::atomic<uint64_t> &counter;
    const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    explicit Timer(std::atomic<uint64_t> &c)
        : counter(c) {}
    ~Timer() {
        counter += static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count());
    }
};

} // namespace renderer::frame_stats
