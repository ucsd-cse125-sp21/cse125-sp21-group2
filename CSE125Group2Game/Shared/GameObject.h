#pragma once
#include <string>

#include "Transform.h"

enum class ObjectType : uint16_t { Default, Player, Enemy, Projectile };

#define NUM_KEYS 4
#define MESSAGE_SIZE 48
#define NAME_LEN 8
#define FLOAT_SIZE 4
#define INT_SIZE 4

class GameObject {
 public:
  static const int KEY_W = 0;
  static const int KEY_A = 1;
  static const int KEY_S = 2;
  static const int KEY_D = 3;

  GameObject(Transform* transform, char* name, int health);

  GameObject(Transform* transform, char* name, int health, ObjectType type);

  void setTransform(Transform* transform);
  void setName(char* name);
  void setHealth(int health);

  Transform* getTransform();
  char* getName();
  int getHealth();

  ObjectType getObjectType();

  uint8_t* serializeInfo();

 private:
  Transform* mTransform;
  char mName[NAME_LEN];
  int mHealth;
  ObjectType mObjectType;
};
