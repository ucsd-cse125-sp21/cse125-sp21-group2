#pragma once
#include "Moveable.h"
#define ENEMY_PROJ_DAMAGE 40
#define PICKUP_CHANCE 1
#define MAX_ENEMY_HEALTH 350

class Enemy : public Moveable {
 public:
  static int enemysSpawned;

  Enemy(Transform* transform, std::string name, int health);

  void update();

  static std::string makeName();

  void setHealth(int amt);

  static Enemy* spawnEnemy();
  bool shouldNotCollide(GameObject* obj);

  static int currHealth;

 private:
  GameObject* GetNearestPlayer();

  glm::vec3 mMoveDirection;
  float rotationSpeed = SPEED_MULTIPLIER * 0.02;

  int mTicksPerUpdate = 3;

  int mTicksTillUpdate = mTicksPerUpdate;

  bool mSpawnPickup = false;
};
