#pragma once
#include "GameObject.h"

#define TOWER_DAMAGE 10
#define TOWER_HEALTH 100

class Tower : public GameObject {
 public:
  static int mTowerSpawned;

  Tower(Transform* transform, std::string name, int health)
      : GameObject(transform, name, health, ObjectType::Tower) {}

  void update();

  static void spawn();
  bool shouldNotCollide(GameObject* obj);

 protected:
  static std::string makeName();
};
