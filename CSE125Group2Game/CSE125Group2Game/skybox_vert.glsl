#version 430 core

layout(location = 0) in vec3 pos;

out vec3 uv;

layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 projection;

void main() {
  uv = pos;
  gl_Position = projection * view * vec4(pos, 1.0);
}
