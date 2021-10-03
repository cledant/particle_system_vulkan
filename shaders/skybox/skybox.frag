#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inFragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 2) uniform samplerCube texSampler;

void main() {
    outColor = texture(texSampler, inFragTexCoord);
}