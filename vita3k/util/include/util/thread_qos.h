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

#ifdef __APPLE__
#include <cstdlib>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <mach/thread_policy.h>
#endif

namespace util {

// Called at the start of the threads the frame rate depends on: guest threads (the games' frame
// timers), the emulated vblank, the render, display and GPU wait threads.
//
// macOS lets ordinary threads' timed waits wake late: by 25% of the wait, up to 5 ms, while the user
// is active, and by 50%, up to 10 ms, when idle (measured on an M5 with macOS 27), whatever the QoS
// class or NSActivityLatencyCritical. A game that waits out a 16 ms frame timer and then flips on
// the next vblank has 0.67 ms to spare, so most frames ran long (Soul Sacrifice Delta: 52-54 fps).
// Real-time threads (THREAD_TIME_CONSTRAINT_POLICY) wake within about 10 us: 60.0 fps. macOS demotes
// a real-time thread that computes past its budget without blocking, so a busy thread (loading)
// doesn't starve the system. VITA3K_QOS=0 leaves the threads as they are. Nothing on other systems.
inline void raise_thread_qos() {
#ifdef __APPLE__
    static const bool enabled = [] {
        const char *const env = std::getenv("VITA3K_QOS");
        return !(env && env[0] == '0');
    }();
    if (!enabled)
        return;
    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);
    const auto ticks = [&](double ms) { return static_cast<uint32_t>(ms * 1e6 * timebase.denom / timebase.numer); };
    // not periodic; up to 5 ms of computation within 10 ms, preemptible
    thread_time_constraint_policy_data_t policy{ 0, ticks(5.0), ticks(10.0), 1 };
    thread_policy_set(mach_thread_self(), THREAD_TIME_CONSTRAINT_POLICY, reinterpret_cast<thread_policy_t>(&policy),
        THREAD_TIME_CONSTRAINT_POLICY_COUNT);
#endif
}

} // namespace util
