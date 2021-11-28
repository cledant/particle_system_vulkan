#include "VulkanRenderer.hpp"

#include <cstdint>
#include <stdexcept>
#include <cassert>
#include <cstring>

#include "VulkanDebug.hpp"
#include "utils/VulkanCommandBuffer.hpp"
#include "utils/VulkanMemory.hpp"
#include "ubo/VulkanUboStructs.hpp"

void
VulkanRenderer::createInstance(std::string &&app_name,
                               std::string &&engine_name,
                               uint32_t app_version,
                               uint32_t engine_version,
                               std::vector<char const *> &&required_extensions)
{
    if constexpr (ENABLE_VALIDATION_LAYER) {
        required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    _appName = std::move(app_name);
    _appVersion = app_version;
    _engineName = std::move(engine_name);
    _engineVersion = engine_version;
    _vkInstance.instance =
      VulkanInstance::createInstance(_appName,
                                     _engineName,
                                     _appVersion,
                                     _engineVersion,
                                     std::move(required_extensions));
}

VkInstance
VulkanRenderer::getVkInstance() const
{
    return (_vkInstance.instance);
}

void
VulkanRenderer::init(VkSurfaceKHR surface, uint32_t win_w, uint32_t win_h)
{
    assert(surface);

    _vkInstance.init(surface);
    _texManager.init(_vkInstance);
    _swapChain.init(_vkInstance, win_w, win_h);
    _sync.init(_vkInstance, _swapChain.swapChainImageViews.size());
    _systemUniform.allocate(_vkInstance.devices,
                             sizeof(SystemUbo) *
                               _swapChain.currentSwapChainNbImg,
                             VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    _ui.init(_vkInstance, _swapChain);
    _skybox.init(_vkInstance,
                 _swapChain,
                 "resources/textures/skybox",
                 "jpg",
                 _texManager,
                 _systemUniform.buffer);
    _particle.init(_vkInstance,
                   _swapChain,
                   DEFAULT_NB_PARTICLES,
                   DEFAULT_PARTICLES_COLOR,
                   _systemUniform.buffer);
    recordRenderCmds();
    _updateComputeCmds = true;
}

void
VulkanRenderer::resize(uint32_t win_w, uint32_t win_h)
{
    vkDeviceWaitIdle(_vkInstance.devices.device);
    if (win_w <= 0 || win_h <= 0) {
        return;
    }

    _swapChain.resize(win_w, win_h);
    _sync.resize(_swapChain.currentSwapChainNbImg);
    _systemUniform.clear();
    _systemUniform.allocate(_vkInstance.devices,
                             sizeof(SystemUbo) *
                               _swapChain.currentSwapChainNbImg,
                             VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    _ui.resize(_swapChain);
    _skybox.resize(_swapChain, _systemUniform.buffer);
    _particle.resize(_swapChain, _systemUniform.buffer);
    recordRenderCmds();
    _updateComputeCmds = true;
}

void
VulkanRenderer::clear()
{
    vkDeviceWaitIdle(_vkInstance.devices.device);
    _skybox.clear();
    _particle.clear();
    _ui.clear();
    _sync.clear();
    _swapChain.clear();
    _texManager.clear();
    _systemUniform.clear();
    _vkInstance.clear();
}

// Info related
std::string const &
VulkanRenderer::getAppName() const
{
    return (_appName);
}

uint32_t
VulkanRenderer::getAppVersion() const
{
    return (_appVersion);
}

std::string const &
VulkanRenderer::getEngineName() const
{
    return (_engineName);
}

uint32_t
VulkanRenderer::getEngineVersion() const
{
    return (_engineVersion);
}

// Skybox related
void
VulkanRenderer::setSkyboxInfo(glm::mat4 const &skyboxInfo)
{
    _skybox.setSkyboxInfo(skyboxInfo);
}

// Particles related
void
VulkanRenderer::toggleParticlesMvt()
{
    _doParticleMvt = !_doParticleMvt;
    _updateComputeCmds = true;
}

void
VulkanRenderer::setParticleGenerationType(VulkanParticleGenerationType type)
{
    switch (type) {
        case VulkanParticleGenerationType::CUBE:
            _randomCompShader = VPCST_RANDOM_CUBE;
            break;
        case VulkanParticleGenerationType::SPHERE:
            _randomCompShader = VPCST_RANDOM_SPHERE;
            break;
        case VulkanParticleGenerationType::DISK:
            _randomCompShader = VPCST_RANDOM_DISK;
            break;
        default:
            _randomCompShader = VPCST_RANDOM_CUBE;
    }
    _doParticleMvt = false;
    _doParticleGeneration = true;
    _updateComputeCmds = true;
}

void
VulkanRenderer::setParticlesNumber(uint32_t nbParticles)
{
    vkDeviceWaitIdle(_vkInstance.devices.device);
    _particle.setParticleNumber(
      nbParticles, _swapChain, _systemUniform.buffer);
    recordRenderCmds();
    _doParticleMvt = false;
    _doParticleGeneration = true;
    _updateComputeCmds = true;
}

void
VulkanRenderer::setParticlesColor(glm::vec3 const &particlesColor)
{
    _particle.setParticlesColor(particlesColor);
}

void
VulkanRenderer::setParticleGravityCenter(glm::vec3 const &particleGravityCenter)
{
    _particle.setParticleGravityCenter(particleGravityCenter);
}

void
VulkanRenderer::setDeltaT(float deltaT)
{
    _particle.setDeltatT(deltaT);
}

// Render Related
void
VulkanRenderer::draw(glm::mat4 const &view_proj_mat)
{
    vkWaitForFences(_vkInstance.devices.device,
                    1,
                    &_sync.inflightFence[_sync.currentFrame],
                    VK_TRUE,
                    UINT64_MAX);

    uint32_t img_index;
    auto result =
      vkAcquireNextImageKHR(_vkInstance.devices.device,
                            _swapChain.swapChain,
                            UINT64_MAX,
                            _sync.imageAvailableSem[_sync.currentFrame],
                            VK_NULL_HANDLE,
                            &img_index);
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        return;
    }

    if (_sync.imgsInflightFence[img_index] != VK_NULL_HANDLE) {
        vkWaitForFences(_vkInstance.devices.device,
                        1,
                        &_sync.imgsInflightFence[img_index],
                        VK_TRUE,
                        UINT64_MAX);
    }
    _sync.imgsInflightFence[img_index] =
      _sync.inflightFence[_sync.currentFrame];

    selectComputeCase();
    emitDrawCmds(img_index, view_proj_mat);

    VkSwapchainKHR swap_chains[] = { _swapChain.swapChain };
    VkSemaphore present_wait_sems[] = {
        _sync.uiFinishedSem[_sync.currentFrame],
    };
    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = present_wait_sems;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swap_chains;
    present_info.pImageIndices = &img_index;
    present_info.pResults = nullptr;
    vkQueuePresentKHR(_vkInstance.queues.presentQueue, &present_info);
    _sync.currentFrame =
      (_sync.currentFrame + 1) % VulkanSync::MAX_FRAME_INFLIGHT;
}

void
VulkanRenderer::recordRenderCmds()
{
    allocateCommandBuffers(_vkInstance.devices.device,
                           _vkInstance.cmdPools.renderCommandPool,
                           _renderCommandBuffers,
                           _swapChain.swapChainImageViews.size());

    size_t i = 0;
    auto const &skybox_render_pass = _skybox.getVulkanSkyboxRenderPass();
    for (auto &it : _renderCommandBuffers) {
        VkCommandBufferBeginInfo cb_begin_info{};
        cb_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cb_begin_info.flags = 0;
        cb_begin_info.pInheritanceInfo = nullptr;
        if (vkBeginCommandBuffer(it, &cb_begin_info) != VK_SUCCESS) {
            throw std::runtime_error("VulkanRenderer: Failed to begin "
                                     "recording render command buffer");
        }

        // Begin render pass values
        std::array<VkClearValue, 2> clear_vals{};
        clear_vals[0].color = { { 0.2f, 0.2f, 0.2f, 1.0f } };
        clear_vals[1].depthStencil = { 1.0f, 0 };
        VkRenderPassBeginInfo rp_begin_info{};
        rp_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        rp_begin_info.renderPass = skybox_render_pass.renderPass;
        rp_begin_info.framebuffer = skybox_render_pass.framebuffers[i];
        rp_begin_info.renderArea.offset = { 0, 0 };
        rp_begin_info.renderArea.extent = _swapChain.swapChainExtent;
        rp_begin_info.clearValueCount = clear_vals.size();
        rp_begin_info.pClearValues = clear_vals.data();
        vkCmdBeginRenderPass(it, &rp_begin_info, VK_SUBPASS_CONTENTS_INLINE);
        // Emit skybox related commands
        _skybox.generateCommands(it, i);
        _particle.generateCommands(it, i);
        vkCmdEndRenderPass(it);
        if (vkEndCommandBuffer(it) != VK_SUCCESS) {
            throw std::runtime_error(
              "VulkanRenderer: Failed to record model command Buffer");
        }
        ++i;
    }
}

void
VulkanRenderer::recordComputeCmds(VulkanParticleComputeShaderType type,
                                  bool registerCmd)
{
    allocateCommandBuffers(_vkInstance.devices.device,
                           _vkInstance.cmdPools.computeCommandPool,
                           _computeCommandBuffers,
                           _swapChain.swapChainImageViews.size());

    VkCommandBufferBeginInfo cb_begin_info{};
    cb_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cb_begin_info.flags = 0;
    cb_begin_info.pInheritanceInfo = nullptr;

    for (auto &it : _computeCommandBuffers) {
        if (vkBeginCommandBuffer(it, &cb_begin_info) != VK_SUCCESS) {
            throw std::runtime_error("VulkanRenderer: Failed to begin "
                                     "recording compute command buffer");
        }
        if (registerCmd) {
            _particle.generateComputeCommands(it, type);
        }
        vkEndCommandBuffer(it);
    }
}

void
VulkanRenderer::selectComputeCase()
{
    if (_updateComputeCmds) {
        if (_doParticleGeneration) {
            recordComputeCmds(_randomCompShader, true);
            _doParticleGeneration = false;
            return;
        }
        if (_doParticleMvt) {
            recordComputeCmds(VPCST_GRAVITY, true);
            _updateComputeCmds = false;
            return;
        }
        if (!_doParticleMvt) {
            recordComputeCmds(VPCST_GRAVITY, false);
            _updateComputeCmds = false;
            return;
        }
    }
}

void
VulkanRenderer::emitDrawCmds(uint32_t img_index, glm::mat4 const &view_proj_mat)
{
    // Update UBOs
    copyOnCpuCoherentMemory(_vkInstance.devices.device,
                            _systemUniform.memory,
                            img_index * sizeof(SystemUbo) +
                              offsetof(SystemUbo, view_proj),
                            sizeof(glm::mat4),
                            &view_proj_mat);
    _skybox.setSkyboxModelMatOnGpu(img_index);
    _particle.setGfxUboOnGpu(img_index);
    _particle.setCompUboOnGpu();

    // Send Compute rendering
    VkSemaphore wait_compute_sems[] = {
        _sync.imageAvailableSem[_sync.currentFrame],
    };
    VkPipelineStageFlags compute_wait_stages[] = {
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
    };
    VkSemaphore finish_compute_sig_sems[] = {
        _sync.computeFinishedSem[_sync.currentFrame],
    };
    VkSubmitInfo compute_submit_info{};
    compute_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    compute_submit_info.pWaitSemaphores = wait_compute_sems;
    compute_submit_info.pWaitDstStageMask = compute_wait_stages;
    compute_submit_info.waitSemaphoreCount = 1;
    compute_submit_info.pSignalSemaphores = finish_compute_sig_sems;
    compute_submit_info.signalSemaphoreCount = 1;
    compute_submit_info.pCommandBuffers = &_computeCommandBuffers[img_index];
    compute_submit_info.commandBufferCount = 1;
    if (vkQueueSubmit(_vkInstance.queues.computeQueue,
                      1,
                      &compute_submit_info,
                      VK_NULL_HANDLE) != VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanRenderer: Failed to submit compute draw command buffer");
    }

    // Send Model rendering
    VkSemaphore wait_model_sems[] = {
        _sync.computeFinishedSem[_sync.currentFrame],
    };
    VkPipelineStageFlags model_wait_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    };
    VkSemaphore finish_model_sig_sems[] = {
        _sync.renderFinishedSem[_sync.currentFrame],
    };
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pWaitSemaphores = wait_model_sems;
    submit_info.pWaitDstStageMask = model_wait_stages;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pSignalSemaphores = finish_model_sig_sems;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pCommandBuffers = &_renderCommandBuffers[img_index];
    submit_info.commandBufferCount = 1;
    if (vkQueueSubmit(
          _vkInstance.queues.graphicQueue, 1, &submit_info, VK_NULL_HANDLE) !=
        VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanRenderer: Failed to submit render draw command buffer");
    }

    // Send Ui rendering
    VkSemaphore wait_ui_sems[] = {
        _sync.renderFinishedSem[_sync.currentFrame],
    };
    VkPipelineStageFlags ui_wait_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    };
    VkSemaphore finish_ui_sig_sems[] = {
        _sync.uiFinishedSem[_sync.currentFrame],
    };
    VkSubmitInfo ui_submit_info{};
    ui_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    ui_submit_info.pWaitSemaphores = wait_ui_sems;
    ui_submit_info.pWaitDstStageMask = ui_wait_stages;
    ui_submit_info.waitSemaphoreCount = 1;
    ui_submit_info.pSignalSemaphores = finish_ui_sig_sems;
    ui_submit_info.signalSemaphoreCount = 1;
    auto ui_cmd_buffer =
      _ui.generateCommandBuffer(img_index, _swapChain.swapChainExtent, false);
    ui_submit_info.commandBufferCount = 1;
    ui_submit_info.pCommandBuffers = &ui_cmd_buffer;
    vkResetFences(
      _vkInstance.devices.device, 1, &_sync.inflightFence[_sync.currentFrame]);
    if (vkQueueSubmit(_vkInstance.queues.graphicQueue,
                      1,
                      &ui_submit_info,
                      _sync.inflightFence[_sync.currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanRenderer: Failed to submit ui draw command buffer");
    }
}
