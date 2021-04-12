#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glad/glad.h"

Camera::Camera(glm::vec3 position, glm::vec3 facing, glm::vec3 up)
    : mPosition(position), mFacing(facing), mUp(up) {
  mView = glm::lookAt(mPosition, mFacing, mUp);
}

void Camera::use() {
  glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(mView));
}
