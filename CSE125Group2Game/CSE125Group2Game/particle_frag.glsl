#version 430 core

in vec3 pColor;
in float flife;
in vec2 uvu;

out vec4 FragColor;

uniform sampler2D tex;

layout(location = 5) uniform bool onlyColor = false;

void main() {
  if (onlyColor) {
    FragColor = vec4(pColor, 1.0);
  } else {
    vec4 color = texture(tex, uvu);
    color.w *= flife;
    FragColor = color;
  }
}
