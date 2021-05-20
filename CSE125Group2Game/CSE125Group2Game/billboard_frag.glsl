#version 430 core

in vec2 uvu;

out vec4 FragColor;

layout(location = 3) uniform vec3 color;

void main() { FragColor = vec4(color, 1.0f); }
