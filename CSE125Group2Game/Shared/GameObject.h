#pragma once
#include <string>

#include "Transform.h"

// TODO: Create moveable class which deals with rotation (each object has its
// own pivot)

enum class ObjectType : uint16_t { Default, Player, Enemy, Projectile };

#define NUM_KEYS 5
#define NAME_LEN 8
#define FLOAT_SIZE 4
#define INT_SIZE sizeof(int)
#define MESSAGE_SIZE NAME_LEN + (12 * FLOAT_SIZE) + INT_SIZE
#define MAX_PLAYERS 4

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

  bool isDefault();
  bool isPlayer();
  bool isEnemy();
  bool isProjectile();

  void setTransform(Transform* transform);
  void addTranslation(glm::vec3 translation);
  void setName(char* name);
  void setHealth(int health);
  glm::vec3 getForwardVector();
  void setForwardVector(glm::vec3 forwardVector);

  Transform* getTransform();

  void virtual update() {}
  bool virtual shouldNotCollide(GameObject* obj) {
    return !strncmp(obj->getName(), "root0000", NAME_LEN) ||
           (obj->getHealth() <= 0 && !obj->isDefault()) ||
           !strncmp(obj->getName(), mName, NAME_LEN);
  }

  // Returns null terminated name
  char* getName();
  int getHealth();

  ObjectType getObjectType();

  bool mIsModified = true;

  static std::string makeName(std::string prefix, int count);

 protected:
  Transform* mTransform;
  char mName[NAME_LEN];
  int mHealth;
  ObjectType mObjectType;
  glm::vec3 mForwardVector;  // could be moved to transform, idk tho
};
