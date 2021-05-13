#pragma once
#include <string>

#include "Transform.h"

// TODO: Create moveable class which deals with rotation (each object has its
// own pivot)

enum class ObjectType : uint16_t { Default, Player, Enemy, Projectile, Tower };

#define NUM_KEYS 5
#define NAME_LEN 8
#define FLOAT_SIZE 4
#define INT_SIZE sizeof(int)
#define TYPE_SIZE sizeof(ObjectType)
#define MESSAGE_SIZE NAME_LEN + (19 * FLOAT_SIZE) + INT_SIZE + TYPE_SIZE
#define MAX_PLAYERS 4

#define RADIUS 25

class GameObject {
 public:
  static const int FORWARD = 0;
  static const int LEFT = 1;
  static const int BACKWARD = 2;
  static const int RIGHT = 3;
  static const int SHOOT = 4;

  GameObject(Transform* transform, const std::string& name, int health);

  GameObject(Transform* transform, const std::string& name, int health,
             ObjectType type);

  ~GameObject();

  bool isDefault();
  bool isPlayer();
  bool isEnemy();
  bool isTower();
  bool isProjectile();

  void setTransform(Transform* transform);
  void addTranslation(glm::vec3 translation);
  void setName(const std::string& name);
  void setHealth(int health);
  Transform* getTransform();

  void virtual update() {}
  bool virtual shouldNotCollide(GameObject* obj) {
    return obj->getName() == "root0000" ||
           (obj->getHealth() <= 0 && !obj->isDefault()) ||
           obj->getName() == mName;
  }

  // Returns null terminated name
  std::string getName();
  int getHealth();

  ObjectType getObjectType();

  bool mIsModified = true;

  static std::string makeName(std::string prefix, int count);

 protected:
  Transform* mTransform;
  // char mName[NAME_LEN];
  std::string mName;
  int mHealth;
  ObjectType mObjectType;
};
