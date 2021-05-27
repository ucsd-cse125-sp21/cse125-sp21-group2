#pragma once
#include <glm/glm.hpp>

#include "MeshLoader.h"

class Rect {
 public:
  Rect(MeshLoader& loader, const glm::vec2& position, const glm::vec2& size,
       const glm::vec4& color);

  Mesh mMesh;
  glm::vec2 mPosition;
  glm::vec2 mSize;
  glm::vec4 mColor;

  glm::mat4 getModelMatrix() const;

 private:
};
