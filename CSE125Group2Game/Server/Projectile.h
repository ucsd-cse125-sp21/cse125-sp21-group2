#pragma once
#include "GameObject.h"

class Projectile : public GameObject {
 public:
  static int mProjectilesSpawned;
  Projectile(Transform* transform, char* name, int health,
             glm::vec3 forwardVector);

  static void spawnProjectile(glm::vec3 translation, glm::vec3 forwardVector);

  void Update();

  std::vector<glm::vec3> getPath();

 protected:
  static std::string makeName();
  void calculatePath();
  // honestly not sure if we would need this / if we wanna change it
  std::vector<glm::vec3> mPath;  // pre compute path on creation and just check
                                 // for collisions as we go ahead
  int mProjectileMaxTicks =
      3;  // how many ticks a projectile should last for unless it collides
  int mProjectileNextTick;
};
