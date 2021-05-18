﻿#pragma once
#include "GameObject.h"

#define TOWER_DAMAGE 10
#define TOWER_HEALTH 1

#define TOWER_HEAL_RATE_MS 5000
#define TOWER_HEAL_AMT 5

class Tower : public GameObject {
 public:
  static int mTowerSpawned;

  Tower(Transform* transform, std::string name, int health)
      : GameObject(transform, name, health, ObjectType::Tower) {}

  void update();

  void setHealth(int amt);

  static void spawn();
  bool shouldNotCollide(GameObject* obj);

 protected:
  static std::string makeName();

  unsigned long mLastHeal;
};
