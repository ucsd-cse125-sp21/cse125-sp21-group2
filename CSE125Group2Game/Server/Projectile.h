﻿#pragma once
#include "Moveable.h"

#define PROJ_SPAWN_RATE_MS 100

class Projectile : public Moveable {
 public:
  static int mProjectilesSpawned;
  static unsigned long mTickLastSpawn;  // [MAX_PLAYERS];

  Projectile(Transform* transform, char* name, int health, GameObject* parent);

  static void spawnProjectile(GameObject* parent);

  void update();
  bool shouldNotCollide(GameObject* obj);

  std::vector<glm::vec3> getPath();

 protected:
  static std::string makeName();
  void calculatePath();
  // honestly not sure if we would need this / if we wanna change it
  std::vector<glm::vec3> mPath;  // pre compute path on creation and just check
                                 // for collisions as we go ahead
  int mProjectileMaxTicks =
      100;  // how many ticks a projectile should last for unless it collides
  int mProjectileNextTick;

  GameObject* mParent;
};
