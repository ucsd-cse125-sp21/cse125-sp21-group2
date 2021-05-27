#version 430 core

in vec2 uv;
out vec4 color;

layout(location = 2) uniform vec4 col;

void main() { color = col; }
