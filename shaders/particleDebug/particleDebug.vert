#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inVertexPosition;

layout(location = 0) out vec3 outFragParticleColor;

layout(binding = 0) uniform SystemUBO {
    mat4 view_proj;
} systemUbo;

layout(binding = 1) uniform ParticleDebugUbo {
    vec3 center;
    vec3 color;
} particleDebugUbo;

void main() {
    gl_Position = systemUbo.view_proj * vec4(inVertexPosition + particleDebugUbo.center, 1.0);
    outFragParticleColor = particleDebugUbo.color;
}