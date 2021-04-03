#version 430 core

layout(location = 0) in vec3 pos;

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 projection;

out vec4 vertColor;

void main() {
    gl_Position = projection * view * model * vec4(pos.x, pos.y, pos.z, 1.0);
    vertColor = vec4(1.0f, 0.0f, 0.0f, 1.0);
}
