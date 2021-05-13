#pragma once
#include "Moveable.h"

class Enemy : public Moveable {
 public:
  static int enemysSpawned;

  Enemy(Transform* transform, std::string name, int health);

  void update();

  static std::string makeName();

 private:
  GameObject* GetNearestPlayer();

  glm::vec3 mMoveDirection;
  float rotationSpeed = 0.02;

  int mTicksPerUpdate = 3;

  int mTicksTillUpdate = mTicksPerUpdate;
};
