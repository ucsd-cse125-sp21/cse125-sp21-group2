﻿#pragma once
#include "Moveable.h"
#include "Projectile.h"
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

  glm::vec3 getRotationSpeed();

 private:
  glm::vec3 mRotationSpeed = glm::vec3(-0.04, -0.08, 0);

  glm::vec3 mVelocity;

  int mPlayerId;
};