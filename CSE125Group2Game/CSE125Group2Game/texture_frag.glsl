#version 430 core

in vec3 fragPos;
in vec3 normal;
in vec2 uvu;

out vec4 FragColor;

uniform sampler2D diffuseMap;

// constants (for now ....)
uniform vec3 lightColor = vec3(0.75f, 0.75f, 0.75f);
uniform vec3 lightPos = vec3(0.0, 0.0, 0.0f);
uniform vec3 viewPos = vec3(0.0, 0.0, 10.0f);

void main() {
  vec3 nnormal = normalize(normal);

  // ambient component
  /*float ambientStrength = 0.1f;
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
  vec3 specular = specStrength * specular;*/

  /* */

  // diffuse component
  vec3 lightVec = -normalize(lightPos - fragPos);
  vec3 diffuse =
      max(dot(nnormal, lightVec), 0.0) * vec3(texture(diffuseMap, uvu));

  FragColor = vec4(lightColor * (diffuse), 1.0f);
}
