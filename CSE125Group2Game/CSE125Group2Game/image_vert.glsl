#version 430 core

layout(location = 0) in vec4 vertex;  // vec2 pos, vec2 tex
layout(location = 1) in vec4 normal;  // ignored
layout(location = 2) in vec2 uv;      //
out vec2 uvu;

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 proj;

void main() {
  gl_Position = proj * model * vec4(vertex.xy, 0.0, 1.0);
  uvu = uv;
}
