#include "Moveable.h"

void Moveable::moveForward() {
  mPivot->addRotation(glm::vec3(rotationSpeed.x, 0, 0));
}

void Moveable::moveBack() {
  mPivot->addRotation(glm::vec3(-rotationSpeed.x, 0, 0));
}

void Moveable::moveLeft() {
  mPivot->addRotation(glm::vec3(0, 0, rotationSpeed.z));
}

void Moveable::moveRight() {
  mPivot->addRotation(glm::vec3(0, 0, -rotationSpeed.z));
}

void Moveable::move(glm::vec3 angle) { mPivot->addRotation(angle); }

glm::vec3 Moveable ::getWorldPositionFromPivot(glm::vec3 modelPos) {
  return glm::vec3(0);
  // TODO: figure out math

  // glm::vec3(mPivot->getModel() * glm::mat4(1));
}
