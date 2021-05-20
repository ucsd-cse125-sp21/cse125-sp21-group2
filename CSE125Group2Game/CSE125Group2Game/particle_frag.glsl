#version 430 core

in float flife;
in vec2 uvu;

out vec4 FragColor;

uniform sampler2D tex;

void main() {
  vec4 color = texture(tex, uvu);
  color.w *= flife;
  FragColor = color;
}
