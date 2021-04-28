#pragma once
#include "GameObject.h"

class Enemy : public GameObject {
 public:
  static bool hasSpawned;

  Enemy(Transform* transform, char* name, int health);

  void Update();

 private:
  GameObject* GetNearestPlayer();

  glm::vec3 mMoveDirection;

  int mTicksPerUpdate = 10;
  glm::vec3 mMoveSpeed = glm::vec3(0.2);

  int mTicksTillUpdate = mTicksPerUpdate;
};
