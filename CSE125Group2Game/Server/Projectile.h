#pragma once
#include <unordered_map>

#include "Moveable.h"

#define PROJ_SPAWN_RATE_MS 100

class Projectile : public Moveable {
 public:
  static int mProjectilesSpawned;

  static std::unordered_map<std::string, unsigned long> mTickLastSpawn;
  // todo: might wanna change to hashmap to map obj name to
  // last tick because players and enemies can have
  // overlapping IDs

  Projectile(Transform* transform, std::string name, int health,
             GameObject* parent);

  static void spawnProjectile(GameObject* parent);
  static void spawnProjectileAngle(GameObject* parent, float angle,
                                   float speedMultiplier = 1);

  void update();
  bool shouldNotCollide(GameObject* obj);

  GameObject* getParent();
  float rotationSpeed = -0.1;
  int mProjectileMaxTicks =
      15;  // how many ticks a projectile should last for unless it collides

 protected:
  static std::string makeName();
  // honestly not sure if we would need this / if we wanna change it
  std::vector<glm::vec3> mPath;  // pre compute path on creation and just check
                                 // for collisions as we go ahead

  int mProjectileNextTick;

  GameObject* mParent;
};
