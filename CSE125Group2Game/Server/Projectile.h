#pragma once
#include "GameObject.h"

class Projectile : public GameObject {
  Projectile(Transform* transform, char* name, int health);

  void calculatePath();

  std::vector<glm::vec3> getPath();

 protected:
  // honestly not sure if we would need this / if we wanna change it
  std::vector<glm::vec3> mPath;  // pre compute path on creation and just check
                                 // for collisions as we go ahead
};
