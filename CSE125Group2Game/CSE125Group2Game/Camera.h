#pragma once

#include <glm/glm.hpp>

class Camera {
 public:
  Camera(glm::vec3 position, glm::vec3 facing, glm::vec3 up);

  void use();

 private:
  glm::vec3 mPosition, mFacing, mUp;

  glm::mat4 mView;
};
