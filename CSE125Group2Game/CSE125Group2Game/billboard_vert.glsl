#version 430 core

// layout(location = 0) in vec2 uv;
layout(location = 0) in vec3 pos;

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 projection;
layout(location = 4) uniform mat4 scale;
// layout(location = 5) uniform mat4 translate;

out vec2 uvu;

void main() {
  // makes the billboard face front
  vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
  vec3 cameraUp = vec3(view[0][1], view[1][1], view[2][1]);

  vec3 particlePos = vec3(model * vec4(0.0, 0.0, 0.0, 1.0));
  vec3 posS = vec3(scale * vec4(pos, 1.0));

  vec3 viewPosWorld = particlePos + cameraRight * posS.x + cameraUp * posS.y;

  // vec4 posViewSpace = view * model * vec4(pos, 1.0);
  // posViewSpace.xy += (pos.xy);
  // gl_Position = projection * posViewSpace;
  gl_Position = projection * view * vec4(viewPosWorld, 1.0);
  // uvu = uv;
}
