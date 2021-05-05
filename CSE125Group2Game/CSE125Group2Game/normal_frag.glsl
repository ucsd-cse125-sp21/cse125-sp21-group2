#version 430 core

in vec3 normal;

out vec4 FragColor;

void main() {
  vec3 nnormal = normalize(normal);

  FragColor = vec4((nnormal + 1.0f) / 2.0f, 1.0f);
}
