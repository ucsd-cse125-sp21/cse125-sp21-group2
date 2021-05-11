#pragma once
#include "GameObject.h"

class Moveable : public GameObject {
 public:
  Moveable(Transform* transform, std::string name, int health)
      : GameObject(transform, name, health) {
    mPivot =
        new Transform(glm::vec3(0), transform->getRotation(), glm::vec3(1));
  }

  Moveable(Transform* transform, std::string name, int health, ObjectType type)
      : GameObject(transform, name, health, type) {
    // TODO: Change 26 to radius

    mPivot =
        new Transform(glm::vec3(0), transform->getRotation(), glm::vec3(1));
  }

  void moveForward();

  void moveBack();

  void moveLeft();

  void moveRight();

  void move(glm::vec3 angle);

  glm::vec3 getWorldPositionFromPivot(glm::vec3 modelPos);

  glm::vec3 getRotationSpeed();

  glm::mat3 rotate(const float degrees, const glm::vec3& axis);

 protected:
  // Can rotate at 5 degrees per tick
  glm::vec3 mRotationSpeed = glm::vec3(-10);

  Transform* mPivot;

  Transform* mModelTransform =
      new Transform(glm::vec3(0, 26, 0), glm::vec3(0), glm::vec3(1));
};
