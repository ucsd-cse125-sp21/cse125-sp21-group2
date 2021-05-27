#include "Rect.h"

#include <glm/ext/matrix_transform.hpp>

Rect::Rect(MeshLoader& loader, const glm::vec2& position, const glm::vec2& size,
           const glm::vec4& color)
    : mPosition(position), mSize(size), mColor(color) {
  mMesh = loader.Square();
}

glm::mat4 Rect::getModelMatrix() const {
  // put rect at -0.5 Z so it is behind all fonts
  glm::mat4 translate =
      glm::translate(glm::mat4(1.0), glm::vec3(mPosition, -0.5));
  return glm::scale(translate, glm::vec3(mSize, 0));
}
