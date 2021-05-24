#pragma once
#include "Moveable.h"
#define ENEMY_PROJ_DAMAGE 40
#define PICKUP_CHANCE 1

class Enemy : public Moveable {
 public:
  static int enemysSpawned;

  Enemy(Transform* transform, std::string name, int health);

  void update();

  static std::string makeName();

  void setHealth(int amt);

  static Enemy* spawnEnemy();
  bool shouldNotCollide(GameObject* obj);

 private:
  GameObject* GetNearestPlayer();

  glm::vec3 mMoveDirection;
  float rotationSpeed = 0.02;

  int mTicksPerUpdate = 3;

  int mTicksTillUpdate = mTicksPerUpdate;

  bool mSpawnPickup = false;
};
