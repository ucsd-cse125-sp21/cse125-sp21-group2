#include "Material.h"

#include "Utils.h"

Material::Material(aiMaterial* other, TextureLoader& tloader) {
  aiReturn res;
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
    // HACK: just assume any mat with texture map also has bump map possibly
    res = other->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texName);
    if (res == aiReturn_SUCCESS) {
      diffuseMap = tloader.loadTexture(ASSET(texName.C_Str()));
    }

    res = other->Get(AI_MATKEY_TEXTURE(aiTextureType_HEIGHT, 0), texName);
    if (res == aiReturn_SUCCESS) {
      normalMap = tloader.loadTexture(ASSET(texName.C_Str()));
    }
  }
}
