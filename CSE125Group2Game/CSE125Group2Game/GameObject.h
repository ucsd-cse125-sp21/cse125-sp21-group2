#pragma once
#include <string>

#include "Transform.h"
class GameObject {
 public:
  GameObject(Transform* transform, std::string name, int health);

  void setTransform(Transform* transform);
  void setName(std::string name);
  void setHealth(int health);

  Transform* getTransform();
  std::string getName();
  int getHealth();

 private:
  Transform* mTransform;
  std::string mName;
  int mHealth;
};
