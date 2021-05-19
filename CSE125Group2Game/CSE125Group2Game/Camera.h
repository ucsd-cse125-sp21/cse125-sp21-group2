#pragma once

#include <glm/glm.hpp>

class Camera {
 public:
  Camera() : mPosition(), mFacing(0.0f, 0.0f, -1.0f), mUp(0.0f, 1.0f, 0.0f) {}
  Camera(glm::vec3 position, glm::vec3 facing, glm::vec3 up);

  void use();

  glm::mat4 getViewMatrix() const;

  void setPosition(const glm::vec3& position);
  void setFacing(const glm::vec3& facing);
  void setUp(const glm::vec3& up);

  glm::vec3 getPosition() const { return mPosition; }

 private:
  glm::vec3 mPosition, mFacing, mUp;
};
