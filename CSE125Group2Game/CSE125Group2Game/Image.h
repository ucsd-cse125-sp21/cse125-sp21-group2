#pragma once
#pragma once
#include <glm/glm.hpp>

#include "MeshLoader.h"
#include "TextureLoader.h"

class Image {
 public:
  Image(MeshLoader& loader, const glm::vec2& position, const glm::vec2& size,
        const Texture& texture);

  Mesh mMesh;
  glm::vec2 mPosition;
  glm::vec2 mSize;
  Texture mTexture;

  glm::mat4 getModelMatrix() const;

 private:
};
