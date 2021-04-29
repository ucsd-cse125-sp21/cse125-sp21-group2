#version 430 core

in vec3 fragPos;
in vec3 normal;

out vec4 FragColor;

layout(location = 3) uniform vec3 ambient;
layout(location = 4) uniform vec3 diffuse;
layout(location = 5) uniform vec3 specular;
layout(location = 6) uniform float shininess = 32;

// constants (for now ....)
uniform vec3 lightColor = vec3(0.5f, 0.5f, 0.5f);
uniform vec3 lightPos = vec3(0.0, 0.0, 10.0f);
uniform vec3 viewPos = vec3(0.0, 0.0, 10.0f);

void main() {
  vec3 nnormal = normalize(normal);

  // ambient component
  float ambientStrength = 0.1f;
  vec3 ambient = ambientStrength * ambient;

  // diffuse component
  vec3 lightVec = normalize(lightPos - fragPos);
  vec3 diffuse = max(dot(nnormal, lightVec), 0.0) * diffuse;

  // specular component
  vec3 viewVec = normalize(viewPos - fragPos);
  // vec3 reflectVec = reflect(-lightVec, nnormal);
  vec3 halfwayVec = normalize(lightVec + viewVec);

  // float specStrength = pow(max(dot(viewVec, reflectVec), 0.0), shininess);
  float specStrength = pow(max(dot(nnormal, halfwayVec), 0.0), shininess);
  vec3 specular = specStrength * specular;

  FragColor = vec4(lightColor * (ambient + diffuse + specular), 1.0f);
}