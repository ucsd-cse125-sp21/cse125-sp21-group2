#version 430 core

in vec3 fragPos;
in vec3 normal;
in vec2 uvu;
in mat3 tbn;

out vec4 FragColor;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

layout(location = 3) uniform vec3 ambient;
layout(location = 4) uniform vec3 diffuse;
layout(location = 5) uniform vec3 specular;
layout(location = 6) uniform float shininess = 32;

// constants (for now ............................)
uniform vec3 lightColor = vec3(0.6f, 0.6f, 0.6f);
uniform vec3 lightPos = vec3(0.0, 0.0, 0.0f);
layout(location = 8) uniform vec3 viewPos = vec3(0.0, 0.0, 10.0f);

void main() {
  vec3 mappedNormal = texture(normalMap, uvu).rgb;
  mappedNormal = mappedNormal * 2.0 - 1.0;
  vec3 nnormal = normalize(tbn * mappedNormal);

  // ambient component
  float ambientStrength = 0.1f;
  vec3 ambient = ambientStrength * ambient;

  // diffuse component
  vec3 lightVec = -normalize(lightPos - fragPos);
  vec3 diffuse = max(dot(nnormal, lightVec), 0.0) * diffuse;

  // specular component
  vec3 viewVec = normalize(viewPos - fragPos);
  // vec3 reflectVec = reflect(-lightVec, nnormal);
  vec3 halfwayVec = normalize(lightVec + viewVec);

  // float specStrength = pow(max(dot(viewVec, reflectVec), 0.0), shininess);
  // TODO: fix this exponent?
  float specStrength = pow(max(dot(nnormal, halfwayVec), 0.0), shininess);
  vec3 specular = specStrength * specular;

  // diffuse texture
  vec3 diffuseMap =
      max(dot(nnormal, lightVec), 0.0) * vec3(texture(diffuseMap, uvu));

  FragColor =
      vec4(lightColor * (diffuse + specular + ambient + diffuseMap), 1.0f);
}
