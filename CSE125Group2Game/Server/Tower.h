#pragma once
#include "GameObject.h"

class Tower : public GameObject {
 public:
  static int mTowerSpawned;

  Tower(Transform* transform, std::string name, int health)
      : GameObject(transform, name, health, ObjectType::Tower) {}

  void update();

  static void spawn();

 protected:
  static std::string makeName();
};
