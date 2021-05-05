#version 430 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 projection;

out vec3 normal;

void main() {
  gl_Position = projection * view * model * vec4(pos, 1.0);

  // TODO: better to calculate inverse before, then send it as uniform
  // normal = mat3(transpose(inverse(model))) * norm;
  normal = norm;
}
