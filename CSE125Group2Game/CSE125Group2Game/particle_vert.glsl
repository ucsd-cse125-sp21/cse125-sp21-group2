#version 430 core

layout(location = 0) in vec2 uv;
layout(location = 1) in vec3 pos;
layout(location = 2) in float life;
layout(location = 3) in vec3 color;

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 projection;
layout(location = 3) uniform float particleSize = 0.5;
layout(location = 4) uniform bool anealSize = true;

out vec3 pColor;
out vec2 uvu;
out float flife;

void main() {
  vec4 posViewSpace = view * model * vec4(pos, 1.0);
  // scale the particle
  float scale = particleSize;
  if (anealSize) {
    scale *= life;
  }

  posViewSpace.xy += scale * (uv.xy - vec2(0.5));
  gl_Position = projection * posViewSpace;
  flife = life;
  uvu = uv;
  pColor = color;
}
