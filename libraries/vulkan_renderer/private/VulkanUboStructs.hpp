#ifndef PARTICLE_SYS_VULKAN_VULKANUBOSTRUCTS_HPP
#define PARTICLE_SYS_VULKAN_VULKANUBOSTRUCTS_HPP

struct SystemUbo final
{
    alignas(16) glm::mat4 view_proj = glm::mat4(1.0);
};

struct SkyboxUbo final
{
    alignas(16) glm::mat4 model = glm::mat4(1.0);
};

#endif // PARTICLE_SYS_VULKAN_VULKANUBOSTRUCTS_HPP
