#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glad/glad.h"

Camera::Camera(glm::vec3 position, glm::vec3 facing, glm::vec3 up)
    : mPosition(position), mFacing(facing), mUp(up) {}

void Camera::use() {}

glm::mat4 Camera::getViewMatrix() const {
  return glm::lookAt(mPosition, mFacing, mUp);
}

void Camera::setPosition(const glm::vec3& position) { mPosition = position; }
void Camera::setFacing(const glm::vec3& facing) { mFacing = facing; }
void Camera::setUp(const glm::vec3& up) { mUp = up; }
