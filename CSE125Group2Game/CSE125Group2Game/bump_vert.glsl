#version 430 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 tangent;

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 projection;

out vec3 normal;
out vec3 fragPos;  // position of the fragment in world space
out vec2 uvu;
out mat3 tbn;  // basis of tangent space

void main() {
  gl_Position = projection * view * model * vec4(pos, 1.0);

  // TODO: better to calculate inverse before, then send it as uniform
  normal = mat3(transpose(inverse(model))) * norm;
  fragPos = vec3(model * vec4(pos, 1.0));

  uvu = uv;

  // calculate bitangent with gram-schmidt process
  // from https://learnopengl.com/Advanced-Lighting/Normal-Mapping
  vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
  vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
  // re-ortho
  T = normalize(T - dot(T, N) * N);
  vec3 B = cross(N, T);

  tbn = mat3(T, B, N);
}
