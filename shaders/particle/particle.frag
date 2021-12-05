#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inFragParticleColor;
layout(location = 1) in vec3 inFragGravityCenter;
layout(location = 2) in vec3 inFragParticlePosition;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 absDistToCenterNorm = abs(normalize(inFragParticlePosition - inFragGravityCenter));
    vec3 colorCvt = normalize(inFragParticleColor + absDistToCenterNorm);
    outColor = vec4(colorCvt, 1.0f);
}