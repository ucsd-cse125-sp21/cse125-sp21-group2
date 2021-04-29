#include "Material.h"

Material::Material(aiMaterial* other) {
  aiColor3D color;
  other->Get(AI_MATKEY_COLOR_AMBIENT, color);
  mAmbient = glm::vec3(color.r, color.g, color.b);
  other->Get(AI_MATKEY_COLOR_DIFFUSE, color);
  mDiffuse = glm::vec3(color.r, color.g, color.b);
  other->Get(AI_MATKEY_COLOR_SPECULAR, color);
  mSpecular = glm::vec3(color.r, color.g, color.b);
  other->Get(AI_MATKEY_SHININESS, mShininess);
}
