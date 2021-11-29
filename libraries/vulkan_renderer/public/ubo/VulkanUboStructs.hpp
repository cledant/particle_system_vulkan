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

struct ParticleGfxUbo final
{
    alignas(16) glm::vec3 center{};
    alignas(16) glm::vec3 color{};
};

struct ParticleComputeUbo final
{
    alignas(16) glm::vec3 genCenter{};
    alignas(8) glm::vec2 range{ -10.0f, 10.0f };
    alignas(8) glm::uvec2 seedX{};
    alignas(8) glm::uvec2 seedY{};
    alignas(8) glm::uvec2 seedZ{};
    alignas(4) uint32_t nbParticles{};
    alignas(4) float deltaT{};
    alignas(4) float particleMass = 5.0f;
    alignas(4) float maxSpeed = 100.0f;
};

#endif // PARTICLE_SYS_VULKAN_VULKANUBOSTRUCTS_HPP
