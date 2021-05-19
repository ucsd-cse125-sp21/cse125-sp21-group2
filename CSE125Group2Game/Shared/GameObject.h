#pragma once
#include <string>

#include "Transform.h"

enum class ObjectType : uint16_t { Default, Player, Enemy, Projectile, Tower };

#define NUM_KEYS 6
#define NAME_LEN 8
#define FLOAT_SIZE 4
#define INT_SIZE sizeof(int)
#define TYPE_SIZE sizeof(ObjectType)
#define MESSAGE_SIZE NAME_LEN + (19 * FLOAT_SIZE) + INT_SIZE + TYPE_SIZE
#define MAX_PLAYERS 4

#define RADIUS 25

#define FORWARD 0
#define LEFT 1
#define BACKWARD 2
#define RIGHT 3
#define SHOOT 4
#define RESTART 5

class GameObject {
 public:
  GameObject(Transform* transform, const std::string& name, int health);

  GameObject(Transform* transform, const std::string& name, int health,
             ObjectType type);

  ~GameObject();

  bool isDefault();
  bool isPlayer();
  bool isEnemy();
  bool isTower();
  bool isProjectile();
  bool isDead();
  bool shouldDelete();

  void setTransform(Transform* transform);
  void addTranslation(glm::vec3 translation);
  void setName(const std::string& name);
  void setHealth(int health);
  Transform* getTransform();

  void virtual update() {}
  bool virtual shouldNotCollide(GameObject* obj) {
    return obj->getName() == "root0000" || obj->isDead() ||
           obj->getObjectType() == getObjectType() || obj->isDefault();
  }

  // Returns null terminated name
  std::string getName();
  int getHealth();

  ObjectType getObjectType();

  bool mIsModified = true;
  bool mShouldRender = true;

  static std::string makeName(std::string prefix, int count);

 protected:
  Transform* mTransform;
  // char mName[NAME_LEN];
  std::string mName;
  int mHealth;
  ObjectType mObjectType;
};
