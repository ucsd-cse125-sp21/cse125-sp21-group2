#include "Material.h"

Material::Material(aiMaterial* other) {
  aiColor3D color;
  other->Get(AI_MATKEY_COLOR_AMBIENT, color);
  ambient = glm::vec3(color.r, color.g, color.b);
  other->Get(AI_MATKEY_COLOR_DIFFUSE, color);
  diffuse = glm::vec3(color.r, color.g, color.b);
  other->Get(AI_MATKEY_COLOR_SPECULAR, color);
  specular = glm::vec3(color.r, color.g, color.b);
  other->Get(AI_MATKEY_SHININESS, shininess);
}
