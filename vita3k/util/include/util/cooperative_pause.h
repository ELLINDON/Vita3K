// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>

// Frontend pause is independent of guest and debugger suspension.
class CooperativePause {
    std::atomic<bool> paused_{ false };
    std::atomic<uint32_t> pauses_{ 0 };
    std::mutex mutex_;
    std::condition_variable changed_;

public:
    bool paused() const {
        return paused_.load(std::memory_order_acquire);
    }

    // how many times pause() was called: a timer can tell whether a pause fell inside what it measured
    uint32_t pause_count() const {
        return pauses_.load(std::memory_order_acquire);
    }

    void pause() {
        const std::lock_guard<std::mutex> lock(mutex_);
        pauses_.fetch_add(1, std::memory_order_acq_rel);
        paused_.store(true, std::memory_order_release);
    }

    void resume() {
        {
            const std::lock_guard<std::mutex> lock(mutex_);
            paused_.store(false, std::memory_order_release);
        }
        changed_.notify_all();
    }

    // The caller publishes cancellation first. Taking the wait mutex prevents a lost wakeup.
    void notify() {
        {
            const std::lock_guard<std::mutex> lock(mutex_);
        }
        changed_.notify_all();
    }

    template <class Cancel>
    bool wait(Cancel cancel) {
        if (!paused())
            return !cancel();
        std::unique_lock<std::mutex> lock(mutex_);
        changed_.wait(lock, [&] { return !paused() || cancel(); });
        return !cancel();
    }
};
