#pragma once
#include <string>

#include "Transform.h"

// Todo: create isDefault() method
enum class ObjectType : uint16_t { Default, Player, Enemy, Projectile };

#define NUM_KEYS 5
#define NAME_LEN 8
#define FLOAT_SIZE 4
#define INT_SIZE sizeof(int)
#define MESSAGE_SIZE NAME_LEN + (12 * FLOAT_SIZE) + INT_SIZE

class GameObject {
 public:
  static const int FORWARD = 0;
  static const int LEFT = 1;
  static const int BACKWARD = 2;
  static const int RIGHT = 3;
  static const int SHOOT = 4;

  GameObject(Transform* transform, char* name, int health,
             glm::vec3 forwardVector);

  GameObject(Transform* transform, char* name, int health, ObjectType type,
             glm::vec3 forwardVector);

  ~GameObject();

  void setTransform(Transform* transform);
  void addTranslation(glm::vec3 translation);
  void setName(char* name);
  void setHealth(int health);
  glm::vec3 getForwardVector();
  void setForwardVector(glm::vec3 forwardVector);

  Transform* getTransform();

  void virtual Update() {}

  // Returns null terminated name
  char* getName();
  int getHealth();

  ObjectType getObjectType();

  bool mIsModified = true;

 protected:
  Transform* mTransform;
  char mName[NAME_LEN];
  int mHealth;
  ObjectType mObjectType;
  glm::vec3 mForwardVector;  // could be moved to transform, idk tho
};
