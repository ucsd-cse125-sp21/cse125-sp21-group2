#include "Material.h"

#include "Utils.h"

Material::Material(aiMaterial* other, TextureLoader& tloader) {
  aiColor3D color;
  other->Get(AI_MATKEY_COLOR_AMBIENT, color);
  mAmbient = glm::vec3(color.r, color.g, color.b);
  other->Get(AI_MATKEY_COLOR_DIFFUSE, color);
  mDiffuse = glm::vec3(color.r, color.g, color.b);
  other->Get(AI_MATKEY_COLOR_SPECULAR, color);
  mSpecular = glm::vec3(color.r, color.g, color.b);
  other->Get(AI_MATKEY_SHININESS, mShininess);

  int numTextures = other->GetTextureCount(aiTextureType_DIFFUSE);
  aiString texName;

  // we have a diffuse texture!
  if (numTextures > 0) {
    other->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texName);
    diffuseMap = tloader.loadTexture(ASSET(texName.C_Str()));
  }
}
