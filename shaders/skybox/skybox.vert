#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inVertexPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 outFragTexCoord;

layout(binding = 0) uniform SystemUBO {
    mat4 view_proj;
} systemUbo;

layout(binding = 1) uniform SkyboxUBO {
    mat4 model;
    vec4 scale;
} skyboxUbo;

void main() {
    vec4 pos = systemUbo.view_proj * skyboxUbo.model * vec4(inVertexPosition * skyboxUbo.scale.xyz, 1.0);
    gl_Position = pos.xyww;
    outFragTexCoord = inVertexPosition;
}