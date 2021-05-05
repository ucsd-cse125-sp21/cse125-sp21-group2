#pragma once
#include "Moveable.h"
class Player : public Moveable {
 public:
  Player(Transform* transform, std::string name, int health, int id);

  void update();

  int getId();

  void setId(int id);

 private:
  glm::vec3 mMoveSpeed = glm::vec3(0.3);

  int mPlayerId;
};
