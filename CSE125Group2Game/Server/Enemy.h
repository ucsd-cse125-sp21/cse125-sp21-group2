﻿#pragma once
#include "GameObject.h"

class Enemy : public GameObject {
 public:
  static int enemysSpawned;

  Enemy(Transform* transform, char* name, int health, glm::vec3 forwardVector);

  void update();

 private:
  GameObject* GetNearestPlayer();

  glm::vec3 mMoveDirection;

  int mTicksPerUpdate = 3;
  glm::vec3 mMoveSpeed = glm::vec3(0.3);

  int mTicksTillUpdate = mTicksPerUpdate;
};
