#pragma once
#include <string>

#include "Transform.h"

enum class ObjectType : uint16_t { Default, Player, Enemy, Projectile };

#define NUM_KEYS 5
#define NAME_LEN 8
#define FLOAT_SIZE 4
#define INT_SIZE sizeof(int)
#define MESSAGE_SIZE NAME_LEN + (9 * FLOAT_SIZE) + INT_SIZE

class GameObject {
 public:
  static const int FORWARD = 0;
  static const int LEFT = 1;
  static const int BACKWARD = 2;
  static const int RIGHT = 3;
  static const int SHOOT = 4;

  GameObject(Transform* transform, char* name, int health);

  GameObject(Transform* transform, char* name, int health, ObjectType type);

  ~GameObject();

  void setTransform(Transform* transform);
  void setName(char* name);
  void setHealth(int health);

  Transform* getTransform();

  // Returns null terminated name
  char* getName();
  int getHealth();

  ObjectType getObjectType();

  bool isModified = true;

 protected:
  Transform* mTransform;
  char mName[NAME_LEN];
  int mHealth;
  ObjectType mObjectType;
};
