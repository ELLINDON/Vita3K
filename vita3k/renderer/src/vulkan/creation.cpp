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

#include <xxh3.h>

#include <renderer/types.h>
#include <renderer/vulkan/functions.h>
#include <renderer/vulkan/gxm_to_vulkan.h>
#include <renderer/vulkan/state.h>

#include <gxm/types.h>

#include <vkutil/vkutil.h>

namespace renderer::vulkan {

VKContext::VKContext(VKState &state, MemState &mem)
    : state(state)
    , mem(mem)
    , vertex_stream_ring_buffer(vk::BufferUsageFlagBits::eVertexBuffer, MiB(/*128*/ 64))
    , index_stream_ring_buffer(vk::BufferUsageFlagBits::eIndexBuffer, MiB(64))
    , vertex_uniform_stream_ring_buffer(vk::BufferUsageFlagBits::eStorageBuffer, MiB(/*256*/ 64))
    , fragment_uniform_stream_ring_buffer(vk::BufferUsageFlagBits::eStorageBuffer, MiB(/*256*/ 64))
    , vertex_info_uniform_buffer(vk::BufferUsageFlagBits::eUniformBuffer, MiB(16))
    , fragment_info_uniform_buffer(vk::BufferUsageFlagBits::eUniformBuffer, MiB(32)) {
    memset(&prev_vert_ublock, 0, sizeof(shader::RenderVertUniformBlock));
    memset(&prev_frag_ublock, 0, sizeof(shader::RenderFragUniformBlock));

    // specify the alignment
    // for the index buffer, we only have 16 or 32bit types
    index_stream_ring_buffer.alignment = sizeof(uint32_t);
    // for the vertex buffer, nothing should need more alignment than a vec4
    vertex_stream_ring_buffer.alignment = 4 * sizeof(float);

    const uint32_t uniform_alignment = static_cast<uint32_t>(state.physical_device_properties.limits.minUniformBufferOffsetAlignment);
    const uint32_t storage_alignment = static_cast<uint32_t>(state.physical_device_properties.limits.minStorageBufferOffsetAlignment);
    vertex_uniform_stream_ring_buffer.alignment = storage_alignment;
    fragment_uniform_stream_ring_buffer.alignment = storage_alignment;
    vertex_info_uniform_buffer.alignment = uniform_alignment;
    fragment_info_uniform_buffer.alignment = uniform_alignment;

    if (state.features.enable_memory_mapping) {
        // use the default buffer
        std::fill_n(vertex_stream_buffers, SCE_GXM_MAX_VERTEX_STREAMS, state.default_buffer.buffer);

        // also initialize the gpu wait thread
        gpu_request_wait_thread = std::thread(&VKContext::wait_thread_function, this, std::ref(mem));
    } else {
        // these are not needed when using memory mapping
        vertex_stream_ring_buffer.create();
        index_stream_ring_buffer.create();
        vertex_uniform_stream_ring_buffer.create();
        fragment_uniform_stream_ring_buffer.create();

        std::fill_n(vertex_stream_buffers, SCE_GXM_MAX_VERTEX_STREAMS, vertex_stream_ring_buffer.handle());
    }

    vertex_info_uniform_buffer.create();
    fragment_info_uniform_buffer.create();

    // default values for the viewport and scissors
    viewport = vk::Viewport{
        .x = 0.f,
        .y = 0.f,
        .width = 960.f * state.res_multiplier,
        .height = 544.f * state.res_multiplier,
        .minDepth = 0.f,
        .maxDepth = 1.f
    };
    scissor = vk::Rect2D{
        .offset = { 0, 0 },
        .extent = { static_cast<uint32_t>(960 * state.res_multiplier), static_cast<uint32_t>(544U * state.res_multiplier) }
    };

    // allocate descriptor pools
    {
        const uint32_t nb_descriptor = state.features.enable_memory_mapping ? 2U : 4U;

        std::array<vk::DescriptorPoolSize, 2> pool_sizes = {
            vk::DescriptorPoolSize{ vk::DescriptorType::eUniformBufferDynamic, 2 },
            vk::DescriptorPoolSize{ vk::DescriptorType::eStorageBufferDynamic, 2 },
        };

        vk::DescriptorPoolCreateInfo descriptor_pool_info{
            // one for the global buffer descriptor, one for the empty descriptor
            .maxSets = 2,
            .poolSizeCount = nb_descriptor / 2,
            .pPoolSizes = pool_sizes.data()
        };

        global_descriptor_pool = state.device.createDescriptorPool(descriptor_pool_info);

        // we can also create the descriptor set now
        vk::DescriptorSetAllocateInfo descr_set_info{
            .descriptorPool = global_descriptor_pool
        };
        descr_set_info.setSetLayouts(state.pipeline_cache.uniforms_layout);
        global_set = state.device.allocateDescriptorSets(descr_set_info)[0];

        descr_set_info.setSetLayouts(state.pipeline_cache.fragment_textures_layout[0]);
        empty_set = state.device.allocateDescriptorSets(descr_set_info)[0];

        // update it now (will not be updated after)
        constexpr uint64_t vert_uniform_size = shader::RenderVertUniformBlockExtended::get_max_size();
        constexpr uint64_t frag_uniform_size = shader::RenderFragUniformBlockExtended::get_max_size();
        std::array<vk::DescriptorBufferInfo, 4> buffers_info = {
            vk::DescriptorBufferInfo{
                .buffer = vertex_info_uniform_buffer.handle(),
                .range = vert_uniform_size },
            vk::DescriptorBufferInfo{
                .buffer = fragment_info_uniform_buffer.handle(),
                .range = frag_uniform_size },
            vk::DescriptorBufferInfo{
                .buffer = vertex_uniform_stream_ring_buffer.handle(),
                // TODO: get max range of buffer
                .range = KB(500) },
            vk::DescriptorBufferInfo{
                .buffer = fragment_uniform_stream_ring_buffer.handle(),
                // TODO: get max range of buffer
                .range = KB(500) },
        };

        std::array<vk::WriteDescriptorSet, 4> write_descr;
        for (uint32_t i = 0; i < 4; i++) {
            write_descr[i] = vk::WriteDescriptorSet{
                .dstSet = global_set,
                .dstBinding = i,
                .dstArrayElement = 0,
                .descriptorType = i < 2 ? vk::DescriptorType::eUniformBufferDynamic : vk::DescriptorType::eStorageBufferDynamic
            };
            write_descr[i].setBufferInfo(buffers_info[i]);
        }

        state.device.updateDescriptorSets(nb_descriptor, write_descr.data(), 0, nullptr);
    }
}

VKContext::~VKContext() {
    if (gpu_request_wait_thread.joinable())
        gpu_request_wait_thread.join();

    for (auto &[addr, vb] : visibility_buffers)
        state.device.destroy(vb.query_pool);
    visibility_buffers.clear();

    state.device.destroy(global_descriptor_pool);
    global_descriptor_pool = nullptr;
}

VKRenderTarget::VKRenderTarget(VKState &state, const SceGxmRenderTargetParams &params)
    : device(state.device)
    , color(static_cast<uint32_t>(params.width * state.res_multiplier), static_cast<uint32_t>(params.height * state.res_multiplier), vk::Format::eR8G8B8A8Unorm)
    , depthstencil(static_cast<uint32_t>(params.width * state.res_multiplier), static_cast<uint32_t>(params.height * state.res_multiplier), state.deep_stencil_use) {
    width = static_cast<uint32_t>(params.width * state.res_multiplier);
    height = static_cast<uint32_t>(params.height * state.res_multiplier);

    vk::ImageUsageFlags color_usage = vk::ImageUsageFlagBits::eColorAttachment;
    if (state.features.support_shader_interlock)
        color_usage |= vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eStorage;
    else
        color_usage |= vk::ImageUsageFlagBits::eInputAttachment | vk::ImageUsageFlagBits::eTransientAttachment;
    color.init_image(color_usage);
    if (params.multisampleMode == SCE_GXM_MULTISAMPLE_4X) {
        // the depth buffer may need to be 4x bigger if we use a texture without downscale
        depthstencil.width *= 2;
        depthstencil.height *= 2;
    }

    depthstencil.init_image(vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransientAttachment);

    // transition images to their right state
    vk::CommandBuffer cmd_buffer = vkutil::create_single_time_command(state.device, state.general_command_pool);
    // color
    color.transition_to_discard(cmd_buffer, vkutil::ImageLayout::ColorAttachmentReadWrite);
    // depth stencil
    depthstencil.transition_to_discard(cmd_buffer, vkutil::ImageLayout::DepthStencilAttachment, vkutil::ds_subresource_range);
    vkutil::end_single_time_command(state.device, state.general_queue, state.general_command_pool, cmd_buffer);

    constexpr uint16_t SCE_GXM_MAX_SCENES_PER_RENDERTARGET = 8;
    // hopefully this will always be enough
    const uint16_t samples_per_frame = std::min<uint16_t>(params.scenesPerFrame + 2, SCE_GXM_MAX_SCENES_PER_RENDERTARGET);

    // the maximum number of fence we will ever need simultaneously is samples_per_frame * MAX_FRAMES_RENDERING
    fences.resize(samples_per_frame * MAX_FRAMES_RENDERING);
    vk::FenceCreateInfo fence_info{};
    for (int i = 0; i < fences.size(); i++)
        fences[i] = state.device.createFence(fence_info);

    for (int i = 0; i < MAX_FRAMES_RENDERING; i++) {
        vk::CommandBufferAllocateInfo buffer_info{
            .commandPool = state.frames[i].render_pool,
            .commandBufferCount = static_cast<uint32_t>(samples_per_frame)
        };
        cmd_buffers[i] = state.device.allocateCommandBuffers(buffer_info);

        buffer_info.commandPool = state.frames[i].prerender_pool;
        pre_cmd_buffers[i] = state.device.allocateCommandBuffers(buffer_info);
    }
}

VKRenderTarget::~VKRenderTarget() {
    for (auto &fence : fences)
        device.destroy(fence);
    fences.clear();
}

bool create(VKState &state, std::unique_ptr<Context> &context, MemState &mem) {
    context = std::make_unique<VKContext>(state, mem);
    return true;
}

bool create(VKState &state, std::unique_ptr<RenderTarget> &rt, const SceGxmRenderTargetParams &params, const FeatureState &features) {
    rt = std::make_unique<VKRenderTarget>(state, params);
    return true;
}

void destroy(VKState &state, std::unique_ptr<RenderTarget> &rt) {
    if (!rt)
        return;

    VKRenderTarget &render_target = *reinterpret_cast<VKRenderTarget *>(rt.get());
    // don't forget to destroy the framebuffers
    state.surface_cache.destroy_associated_framebuffers(&render_target);

    // deferred destroy everything in case some object is still being used
    FrameObject &frame = state.frame();
    frame.destroy_queue.add_image(render_target.color);
    frame.destroy_queue.add_image(render_target.depthstencil);

    for (auto &fence : render_target.fences)
        frame.destroy_queue.add(fence);
    for (int i = 0; i < MAX_FRAMES_RENDERING; i++) {
        for (auto cmd_buffer : render_target.cmd_buffers[i])
            frame.destroy_queue.add_cmd_buffer(cmd_buffer, state.frames[i].render_pool);

        for (auto cmd_buffer : render_target.pre_cmd_buffers[i])
            frame.destroy_queue.add_cmd_buffer(cmd_buffer, state.frames[i].prerender_pool);
    }
}

bool create(std::unique_ptr<VertexProgram> &vp, VKState &state, const SceGxmProgram &program) {
    vp = std::make_unique<VertexProgram>();

    if (program.program_flags & SCE_GXM_PROGRAM_FLAG_BUFFER_STORE)
        state.has_shader_store = true;

    return true;
}

bool create(std::unique_ptr<FragmentProgram> &fp, VKState &state, const SceGxmProgram &program, const SceGxmBlendInfo *blend) {
    fp = std::make_unique<VKFragmentProgram>();

    VKFragmentProgram *fp_vk = reinterpret_cast<VKFragmentProgram *>(fp.get());

    // Translate blending.
    // programs using native color can't use traditional blending
    if (blend != nullptr && !program.is_native_color()) {
        vk::ColorComponentFlags color_mask{};
        if (blend->colorMask & SCE_GXM_COLOR_MASK_R)
            color_mask |= vk::ColorComponentFlagBits::eR;
        if (blend->colorMask & SCE_GXM_COLOR_MASK_G)
            color_mask |= vk::ColorComponentFlagBits::eG;
        if (blend->colorMask & SCE_GXM_COLOR_MASK_B)
            color_mask |= vk::ColorComponentFlagBits::eB;
        if (blend->colorMask & SCE_GXM_COLOR_MASK_A)
            color_mask |= vk::ColorComponentFlagBits::eA;

        fp_vk->blending = vk::PipelineColorBlendAttachmentState{
            .blendEnable = (blend->colorFunc != SCE_GXM_BLEND_FUNC_NONE) || (blend->alphaFunc != SCE_GXM_BLEND_FUNC_NONE),
            .srcColorBlendFactor = translate_blend_factor(blend->colorSrc),
            .dstColorBlendFactor = translate_blend_factor(blend->colorDst),
            .colorBlendOp = translate_blend_func(blend->colorFunc),
            .srcAlphaBlendFactor = translate_blend_factor(blend->alphaSrc),
            .dstAlphaBlendFactor = translate_blend_factor(blend->alphaDst),
            .alphaBlendOp = translate_blend_func(blend->alphaFunc),
            .colorWriteMask = color_mask
        };
    } else {
        // default values, only blendEnable and colorWriteMask are useful
        fp_vk->blending = vk::PipelineColorBlendAttachmentState{
            .blendEnable = VK_FALSE,
            .srcColorBlendFactor = vk::BlendFactor::eOne,
            .dstColorBlendFactor = vk::BlendFactor::eZero,
            .colorBlendOp = vk::BlendOp::eAdd,
            .srcAlphaBlendFactor = vk::BlendFactor::eOne,
            .dstAlphaBlendFactor = vk::BlendFactor::eZero,
            .alphaBlendOp = vk::BlendOp::eAdd,
            .colorWriteMask = vk::ColorComponentFlagBits::eR
                | vk::ColorComponentFlagBits::eG
                | vk::ColorComponentFlagBits::eB
                | vk::ColorComponentFlagBits::eA
        };
    }

    // a program that reads the framebuffer only for a final SOP2 blend (FeatureState::native_shader_blend): that blend,
    // done by the fixed-function blender, and no read (ss-delta #17)
    // VITA3K_SHADER_BLEND_ONLY=<hash prefix>,...: only these programs (debugging aid)
    static const std::string blend_only = [] {
        const char *v = std::getenv("VITA3K_SHADER_BLEND_ONLY");
        return std::string(v ? v : "");
    }();
    const auto listed = [&]() {
        if (blend_only.empty())
            return true;
        const std::string text = hex_string(sha256(&program, program.size));
        for (size_t start = 0; start <= blend_only.size();) {
            const size_t end = std::min(blend_only.find(',', start), blend_only.size());
            if (end > start && text.rfind(blend_only.substr(start, end - start), 0) == 0)
                return true;
            start = end + 1;
        }
        return false;
    };
    if (state.features.native_shader_blend && program.is_frag_color_used() && !fp_vk->blending.blendEnable && listed()) {
        if (const auto blend = shader::find_shader_blend(program, state.features)) {
            using BF = vk::BlendFactor;
            // a SOP2 selector names src1 or src2 (as a colour or its alpha); the program's colour is Vulkan's source,
            // the old pixel its destination
            const bool swapped = blend->swapped;
            const auto factor = [swapped](bool of_src1, bool alpha, bool complement) {
                const bool source = of_src1 != swapped; // the colour (Vulkan's source)?
                if (alpha)
                    return source ? (complement ? BF::eOneMinusSrcAlpha : BF::eSrcAlpha) : (complement ? BF::eOneMinusDstAlpha : BF::eDstAlpha);
                return source ? (complement ? BF::eOneMinusSrcColor : BF::eSrcColor) : (complement ? BF::eOneMinusDstColor : BF::eDstColor);
            };
            const auto color_factor = [&](uint8_t sel, bool complement) {
                switch (sel) {
                case 1: return factor(true, false, complement);
                case 2: return factor(false, false, complement);
                case 3: return factor(true, true, complement);
                case 4: return factor(false, true, complement);
                default: return complement ? BF::eOne : BF::eZero;
                }
            };
            const auto alpha_factor = [&](uint8_t sel, bool complement) {
                switch (sel) {
                case 1: return factor(true, true, complement);
                case 2: return factor(false, true, complement);
                default: return complement ? BF::eOne : BF::eZero;
                }
            };
            // factor1 applies to src1, factor2 to src2; subtract is src1 minus src2
            const BF c1 = color_factor(blend->csel1, blend->cmod1), c2 = color_factor(blend->csel2, blend->cmod2);
            const BF a1 = alpha_factor(blend->asel1, blend->amod1), a2 = alpha_factor(blend->asel2, blend->amod2);
            const vk::BlendOp subtract = swapped ? vk::BlendOp::eReverseSubtract : vk::BlendOp::eSubtract;
            fp_vk->blending.blendEnable = VK_TRUE;
            fp_vk->blending.srcColorBlendFactor = swapped ? c2 : c1;
            fp_vk->blending.dstColorBlendFactor = swapped ? c1 : c2;
            fp_vk->blending.colorBlendOp = blend->cop == 0 ? vk::BlendOp::eAdd : subtract;
            fp_vk->blending.srcAlphaBlendFactor = swapped ? a2 : a1;
            fp_vk->blending.dstAlphaBlendFactor = swapped ? a1 : a2;
            fp_vk->blending.alphaBlendOp = blend->aop == 0 ? vk::BlendOp::eAdd : subtract;
            fp_vk->native_blend = blend;
        }
    }

    // compute blending hash, as it will be used for the pipeline hash
    fp_vk->blending_hash = XXH_INLINE_XXH3_64bits(&fp_vk->blending, sizeof(vk::PipelineColorBlendAttachmentState));
    return true;
}

} // namespace renderer::vulkan
