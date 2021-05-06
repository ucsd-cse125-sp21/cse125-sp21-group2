#pragma once
#include "GameObject.h"

class Moveable : public GameObject {
 public:
  Moveable(Transform* transform, std::string name, int health)
      : GameObject(transform, name, health) {}

  Moveable(Transform* transform, std::string name, int health, ObjectType type)
      : GameObject(transform, name, health, type) {}

  void moveForward();

  void moveBack();

  void moveLeft();

  void moveRight();

  void move(glm::vec3 angle);

  glm::vec3 getWorldPositionFromPivot(glm::vec3 modelPos);

 private:
  // Can rotate at 5 degrees per tick
  glm::vec3 rotationSpeed = glm::vec3(5);

  Transform* mPivot = new Transform(glm::vec3(0), glm::vec3(0), glm::vec3(1));
};
