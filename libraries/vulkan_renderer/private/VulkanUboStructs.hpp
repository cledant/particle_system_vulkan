#ifndef PARTICLE_SYS_VULKAN_VULKANUBOSTRUCTS_HPP
#define PARTICLE_SYS_VULKAN_VULKANUBOSTRUCTS_HPP

struct SystemUbo final
{
    alignas(16) glm::mat4 view_proj{};
};

struct ModelPipelineUbo final
{
    alignas(16) glm::vec3 diffuse_color{};
    alignas(16) glm::vec3 specular_color{};
    alignas(16) float shininess{};
};

#endif // PARTICLE_SYS_VULKAN_VULKANUBOSTRUCTS_HPP
