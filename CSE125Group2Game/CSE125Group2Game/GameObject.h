#pragma once
#include <string>

#include "Transform.h"

enum class ObjectType : uint16_t {
  Default,
  Player,
  Enemy,
};

class GameObject {
 public:
  GameObject(Transform* transform, std::string name, int health);

  void setTransform(Transform* transform);
  void setName(std::string name);
  void setHealth(int health);

  Transform* getTransform();
  std::string getName();
  int getHealth();

  ObjectType getObjectType();

 private:
  Transform* mTransform;
  std::string mName;
  int mHealth;
  ObjectType mObjectType;
};
