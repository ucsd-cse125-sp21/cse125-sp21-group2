﻿#pragma once
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

  int mTicksPerUpdate = 3;
  glm::vec3 mMoveSpeed = glm::vec3(0.3);

  int mTicksTillUpdate = mTicksPerUpdate;
};
