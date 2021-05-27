#version 430 core

in vec2 uvu;
out vec4 color;

uniform sampler2D tex;

void main() { color = texture(tex, uvu); }
