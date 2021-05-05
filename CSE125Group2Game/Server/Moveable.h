#pragma once
#include "GameObject.h"

class Moveable : public GameObject {
 public:
  Moveable(Transform* transform, char* name, int health,
           glm::vec3 forwardVector)
      : GameObject(transform, name, health, forwardVector) {}

  Moveable(Transform* transform, char* name, int health, ObjectType type,
           glm::vec3 forwardVector)
      : GameObject(transform, name, health, type, forwardVector) {}
};
