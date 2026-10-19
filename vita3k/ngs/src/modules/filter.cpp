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

#include "biquad.h"

#include <ngs/modules/filter.h>
#include <util/log.h>

#include <cstring>
#include <vector>

namespace ngs {

namespace {

struct FilterRuntimeState : public ModuleRuntimeState {
    BiquadState state;
};

} // namespace

std::unique_ptr<ModuleRuntimeState> FilterModule::create_runtime_state() const {
    return std::make_unique<FilterRuntimeState>();
}

void FilterModule::on_state_change(const MemState &mem, ModuleData &data, const VoiceState previous) {
    // a new play starts without the old samples in the filter
    if (data.parent->state == VOICE_STATE_ACTIVE && previous == VOICE_STATE_AVAILABLE)
        data.get_runtime_state<FilterRuntimeState>()->state.clear();
}

void FilterModule::set_default_preset(const MemState &mem, ModuleData &data) {
    if (!data.parent->rack->system->dsp)
        return; // other games keep what they had
    // off (the shape of Killzone Mercenary's own "off" setting; the hardware's default is a **hypothesis**)
    if (SceNgsFilterParams *const p = data.get_parameters<SceNgsFilterParams>(mem))
        *p = { { SCE_NGS_FILTER_PARAMS_STRUCT_ID, sizeof(SceNgsFilterParams) }, { SCE_NGS_FILTER_MODE_OFF, 1000.0f, 0.7f, 1.0f } };
}

bool FilterModule::process(KernelState &kern, const MemState &mem, const SceUID thread_id, ModuleData &data, std::unique_lock<std::recursive_mutex> &scheduler_lock, std::unique_lock<std::mutex> &voice_lock) {
    Voice *const voice = data.parent;
    const System *const system = voice->rack->system;
    // The voices that end in two filters (simple and SCREAM ones) have two sends: the first filter
    // makes output 0 and the second output 1, both from the same signal. The first leaves a copy of
    // its input in the second's scratch, and the second filters that copy.
    const auto &modules = voice->rack->modules;
    const bool send_1 = system->dsp && data.index + 1 < modules.size() && dynamic_cast<FilterModule *>(modules[data.index + 1].get());
    const bool send_2 = system->dsp && data.index > 0 && dynamic_cast<FilterModule *>(modules[data.index - 1].get());
    float *pcm = reinterpret_cast<float *>(voice->products[0].data);
    if (send_1 && pcm && system->granularity > 0) {
        std::vector<uint8_t> &copy = voice->datas[data.index + 1].scratch_data;
        copy.resize(static_cast<size_t>(system->granularity) * sizeof(float) * 2);
        std::memcpy(copy.data(), pcm, copy.size());
    } else if (send_2) {
        pcm = data.scratch_data.empty() || !pcm ? nullptr : reinterpret_cast<float *>(data.scratch_data.data());
    }

    if (!data.is_bypassed) {
        const SceNgsParamsDescriptor *desc = data.get_parameters<SceNgsParamsDescriptor>(mem);
        if (desc && (desc->id == SCE_NGS_FILTER_PARAMS_COEFF_STRUCT_ID || desc->id == SCE_NGS_FILTER_PARAMS_STRUCT_ID)) {
            if (!system->dsp) {
                LOG_WARN_ONCE("Game is using unimplemented filter audio module");
            } else if (pcm && system->granularity > 0) {
                const Biquad q = desc->id == SCE_NGS_FILTER_PARAMS_STRUCT_ID
                    ? biquad_from_params(data.get_parameters<SceNgsFilterParams>(mem)->params, static_cast<float>(system->sample_rate))
                    : biquad_from_coefficients(data.get_parameters<SceNgsFilterParamsCoEff>(mem)->params);
                biquad_process(q, data.get_runtime_state<FilterRuntimeState>()->state, pcm, system->granularity);
            }
        }
    }

    // Definitions with filters have 2 outputs
    if (send_2 && pcm)
        voice->products[1].data = reinterpret_cast<uint8_t *>(pcm);
    else
        voice->products[1] = voice->products[0];

    return false;
}
} // namespace ngs
