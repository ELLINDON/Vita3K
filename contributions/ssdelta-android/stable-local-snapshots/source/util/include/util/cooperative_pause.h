// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <utility>

// Session pause is orthogonal to guest/debugger suspension. Wait without unwinding guest frames.
class CooperativePause {
    std::atomic<bool> paused_{false};
    std::mutex mutex_;
    std::condition_variable changed_;
public:
    bool paused() const { return paused_.load(std::memory_order_acquire); }
    void pause() { std::lock_guard<std::mutex> lock(mutex_); paused_.store(true,std::memory_order_release); }
    void resume() {
        { std::lock_guard<std::mutex> lock(mutex_); paused_.store(false,std::memory_order_release); }
        changed_.notify_all();
    }
    // Cancellation changes outside this mutex; synchronizing here prevents a lost wake-up.
    void notify() { { std::lock_guard<std::mutex> lock(mutex_); } changed_.notify_all(); }
    template<class Cancel> bool wait(Cancel cancel) {
        if (!paused()) return !cancel();
        std::unique_lock<std::mutex> lock(mutex_);
        changed_.wait(lock,[&]{return !paused() || cancel();});
        return !cancel();
    }
    // Hold admission across an audio upload so pause cannot race a final enqueue.
    template<class Cancel,class Action> bool admit(Cancel cancel,Action action) {
        std::unique_lock<std::mutex> lock(mutex_);
        changed_.wait(lock,[&]{return !paused() || cancel();});
        if(cancel())return false;
        action();
        return true;
    }
};
