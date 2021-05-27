#include "Image.h"

#include <glm/ext/matrix_transform.hpp>

Image::Image(MeshLoader& loader, const glm::vec2& position,
             const glm::vec2& size, const Texture& texture)
    : mPosition(position), mSize(size), mTexture(texture) {
  mMesh = loader.Square();
}

glm::mat4 Image::getModelMatrix() const {
  // put rect at -0.3 Z so it is behind all fonts but in front of rects
  glm::mat4 translate =
      glm::translate(glm::mat4(1.0), glm::vec3(mPosition, -0.3));
  return glm::scale(translate, glm::vec3(mSize, 0));
}
