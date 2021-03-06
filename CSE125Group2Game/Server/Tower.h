#pragma once
#include "GameObject.h"

#define TOWER_DAMAGE 20

#define TOWER_HEAL_RATE_MS 10000
#define TOWER_HEAL_AMT 5
#define MAX_TOWER_HEAL_RADIUS 20
#define MID_TOWER_HEAL_RADIUS 15
#define MIN_TOWER_HEAL_RADIUS 10

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
