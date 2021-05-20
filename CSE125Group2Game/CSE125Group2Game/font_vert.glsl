#version 430 core

layout(location = 0) in vec4 vertex;  // vec2 pos, vec2 tex
out vec2 uv;

layout(location = 0) uniform mat4 proj;

void main() {
  gl_Position = proj * vec4(vertex.xy, 0.0, 1.0);
  uv = vertex.zw;
}