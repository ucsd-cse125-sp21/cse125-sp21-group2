#pragma once
#include "Moveable.h"

class Cloud : public Moveable {
 public:
  Cloud(Transform* transform, std::string name, int health);

  void update();

  static void spawn();
  bool shouldNotCollide(GameObject* obj);
};
