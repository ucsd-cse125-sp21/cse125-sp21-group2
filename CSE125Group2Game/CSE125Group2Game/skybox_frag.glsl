#version 430 core

in vec3 uv;

out vec4 FragColor;

uniform samplerCube skybox;

void main() { FragColor = texture(skybox, uv); }