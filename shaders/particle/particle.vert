#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inVertexPosition;

layout(location = 0) out vec3 outFragParticleColor;
layout(location = 1) out vec3 outFragGravityCenter;
layout(location = 2) out vec3 outFragParticlePosition;

layout(binding = 0) uniform SystemUBO {
    mat4 view_proj;
} systemUbo;

layout(binding = 1) uniform ParticleUbo {
    vec3 center;
    vec3 color;
} particleUbo;

void main() {
    gl_Position = systemUbo.view_proj * vec4(inVertexPosition, 1.0);
    outFragParticlePosition = inVertexPosition;
    outFragParticleColor = particleUbo.color;
    outFragGravityCenter = particleUbo.center;
}