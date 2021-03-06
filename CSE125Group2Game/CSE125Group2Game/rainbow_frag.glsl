#version 430 core
#define RAINBOW_SCALE 30.0f
#define SPEED_SCALE 10.0f

in vec3 Pos;

out vec4 FragColor;

layout(location = 7) uniform float time = 1.0;

vec3 hsb2rgb(in vec3 c) {
  vec3 rgb = clamp(abs(mod(c.x * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0,
                   0.0, 1.0);
  rgb = rgb * rgb * (3.0 - 2.0 * rgb);
  return c.z * mix(vec3(1.0), rgb, c.y);
}

void main() {
  float hue = mod(Pos.x + SPEED_SCALE * time, RAINBOW_SCALE) / RAINBOW_SCALE;
  FragColor = vec4(hsb2rgb(vec3(hue, 1.0f, 1.0)), 1.0);
}
