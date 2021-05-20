#pragma once

#include <glm/glm.hpp>
#include <queue>
#define LAG 30

class Camera {
 public:
  Camera() : mPosition(), mFacing(0.0f, 0.0f, -1.0f), mUp(0.0f, 1.0f, 0.0f) {
    for (int i = 0; i < LAG; i++) {
      modelQueue.push(getViewMatrix());
    }
  }
  Camera(glm::vec3 position, glm::vec3 facing, glm::vec3 up);

  void use();

  glm::mat4 getViewMatrix() const;

  void setPosition(const glm::vec3& position);
  void setFacing(const glm::vec3& facing);
  void setUp(const glm::vec3& up);

  glm::vec3 getPosition() const { return mPosition; }

  std::queue<glm::mat4> modelQueue = std::queue<glm::mat4>();

 private:
  glm::vec3 mPosition, mFacing, mUp;
};
