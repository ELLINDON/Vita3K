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

#include <audio/state.h>

#include <audio/impl/cubeb_audio.h>
#include <audio/impl/sdl_audio.h>

#include <util/log.h>

#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <thread>

namespace {

// used when the host has no audio device (none plugged in, or no default one): the ports take the sound
// at the pace a device would and drop it, so a game runs silent instead of stopping
struct SilentAudioOutPort : AudioOutPort {
    std::chrono::steady_clock::time_point next{};
};

class SilentAudioAdapter : public AudioAdapter {
public:
    explicit SilentAudioAdapter(AudioState &audio_state)
        : AudioAdapter(audio_state) {}

    bool init() override {
        return true;
    }

    AudioOutPortPtr open_port(int nb_channels, int freq, int nb_sample) override {
        auto port = std::make_shared<SilentAudioOutPort>();
        port->len_bytes = nb_sample * nb_channels * static_cast<int>(sizeof(int16_t));
        port->len_microseconds = (nb_sample * 1'000'000ULL) / freq;
        return port;
    }

    // one buffer may be ahead of the clock, as in a device's queue; a sleep that overshoots is made up
    // by the next ones, unless the game fell behind by more than a few buffers (a pause, a loading)
    void audio_output(AudioOutPort &out_port, const void *buffer) override {
        auto &port = static_cast<SilentAudioOutPort &>(out_port);
        const auto len = std::chrono::microseconds(port.len_microseconds);
        const auto now = std::chrono::steady_clock::now();
        if (port.next + 4 * len < now)
            port.next = now;
        port.next += len;
        std::this_thread::sleep_until(port.next - len);
    }
};

} // namespace

bool AudioState::init(const std::string &adapter_name) {
    set_backend(adapter_name);
    if (!adapter)
        return false;

    return true;
}

void AudioState::stop_all_ports() {
    {
        const std::lock_guard<std::mutex> lock(mutex);
        for (auto &[_, port] : out_ports) {
            port->stopping = true;
        }
    }
    if (adapter)
        adapter->wake_all_ports();
}

void AudioState::deinit() {
    stop_all_ports();

    const std::lock_guard<std::mutex> lock(mutex);

    out_ports.clear();

    if (in_port.running) {
        SDL_DestroyAudioStream(static_cast<SDL_AudioStream *>(in_port.id));
        in_port.id = nullptr;
        in_port.running = false;
        in_port.len_bytes = 0;
    }

    next_port_id = 1;
}

void AudioState::set_backend(const std::string &adapter_name) {
    if (adapter_name == this->audio_backend)
        return;

    // first delete all ports then delete the backend
    out_ports.clear();
    adapter.reset();
    if (adapter_name == "SDL") {
        adapter = std::make_unique<SDLAudioAdapter>(*this);
    } else if (adapter_name == "Cubeb") {
        adapter = std::make_unique<CubebAudioAdapter>(*this);
    } else {
        LOG_ERROR("Unknown audio adapter {}", adapter_name);
        return;
    }
    this->audio_backend = adapter_name;

    // lock the mutex to make sure nothing happens until the initialisation is done
    const std::lock_guard<std::mutex> lock(mutex);
    if (!adapter->init()) {
        LOG_WARN("No audio device: the game runs without sound");
        adapter = std::make_unique<SilentAudioAdapter>(*this);
        adapter->init();
    }
}

AudioOutPortPtr AudioState::open_port(int nb_channels, int freq, int nb_sample) {
    AudioOutPortPtr port = adapter->open_port(nb_channels, freq, nb_sample);
    set_volume(*port, port->volume);
    return port;
}

void AudioState::audio_output(AudioOutPort &out_port, const void *buffer) {
    if (out_port.stopping)
        return;

    adapter->audio_output(out_port, buffer);

    if (out_port.stopping)
        return;

    uint64_t now = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    uint64_t diff = now - out_port.last_output;
    uint64_t to_wait = out_port.len_microseconds - diff;
    if (diff < out_port.len_microseconds && to_wait > 1000) {
        // This is what we should be waiting to be perfectly accurate
        // However, doing so would cause the host audio buffer to often lack samples to output
        // This is because the PS Vita and the host audio parameters do not match exactly
        // So instead only wait 50% of the time
        // also don't sleep for less than 0.5 ms
        to_wait /= 2;
        std::this_thread::sleep_for(std::chrono::microseconds(to_wait));
        out_port.last_output = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    } else {
        out_port.last_output = now;
    }
}

void AudioState::set_volume(AudioOutPort &out_port, float volume) {
    out_port.volume = volume;
    adapter->set_volume(out_port, volume * global_volume);
}

void AudioState::set_global_volume(float volume) {
    global_volume = volume;
    //  Update adapter volume for each port.
    const std::lock_guard lock(mutex);
    for (const auto &[_, port] : out_ports) {
        adapter->set_volume(*port, port->volume * volume);
    }
}

void AudioState::switch_state(const bool pause) {
    adapter->switch_state(pause);
}

int AudioState::get_rest_sample(AudioOutPort &out_port) {
    return adapter->get_rest_sample(out_port);
}

void AudioState::wake_all_ports() {
    const std::lock_guard<std::mutex> lock(mutex);
    for (auto &[_, port] : out_ports) {
        port->stopping = true;
    }
    if (adapter)
        adapter->wake_all_ports();
}