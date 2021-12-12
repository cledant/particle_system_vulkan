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

constexpr glm::vec3 const DEFAULT_PARTICLES_GRAVITY_CENTER{};
constexpr glm::vec3 const DEFAULT_PARTICLES_COLOR{ 0.0f, 0.5f, 0.3f };
struct ParticleGfxUbo final
{
    alignas(16) glm::vec3 center = DEFAULT_PARTICLES_GRAVITY_CENTER;
    alignas(16) glm::vec3 color = DEFAULT_PARTICLES_COLOR;
};

constexpr uint32_t const DEFAULT_NB_PARTICLES = 1000000;
constexpr float const DEFAULT_PARTICLE_MAX_SPEED = 100.0f;
constexpr float const DEFAULT_PARTICLE_MASS = 5.0f;
struct ParticleComputeUbo final
{
    alignas(16) glm::vec3 genCenter = DEFAULT_PARTICLES_GRAVITY_CENTER;
    alignas(8) glm::vec2 range{ -10.0f, 10.0f };
    alignas(8) glm::uvec2 seedX{};
    alignas(8) glm::uvec2 seedY{};
    alignas(8) glm::uvec2 seedZ{};
    alignas(4) uint32_t nbParticles = DEFAULT_NB_PARTICLES;
    alignas(4) float deltaT{};
    alignas(4) float particleMass = DEFAULT_PARTICLE_MASS;
    alignas(4) float maxSpeed = DEFAULT_PARTICLE_MAX_SPEED;
};

#endif // PARTICLE_SYS_VULKAN_VULKANUBOSTRUCTS_HPP
