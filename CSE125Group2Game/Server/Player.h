#pragma once
#include "Moveable.h"
class Player : public Moveable {
 public:
  Player(Transform* transform, std::string name, int health, int id);

  void update();

  int getId();

  void setId(int id);

  int getVsp();
  int getHsp();

  void setVelocity(glm::vec3 velocity);
  glm::vec3 getVelocity();

  static std::string makeName(int id);

 private:
  glm::vec3 mMoveSpeed = glm::vec3(0.3);
  glm::vec3 mVelocity;
  // int mVsp;
  // int mHsp;

  int mPlayerId;
};
