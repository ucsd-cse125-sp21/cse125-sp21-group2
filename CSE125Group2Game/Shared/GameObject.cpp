#include "GameObject.h"

#include <iostream>

GameObject::GameObject(Transform* transform, const std::string& name,
                       int health)
    : mTransform(transform),
      mHealth(health),
      mObjectType(ObjectType::Default),
      mName(name) {
  mIsModified = true;
}

GameObject::GameObject(Transform* transform, const std::string& name,
                       int health, ObjectType type)
    : mTransform(transform), mHealth(health), mObjectType(type), mName(name) {
  mIsModified = true;
}

GameObject::~GameObject() { delete mTransform; }

void GameObject::setTransform(Transform* transform) { mTransform = transform; }
void GameObject::addTranslation(glm::vec3 translation) {
  // Only modifiy translation if it is non-zero
  if (glm::length(translation) == 0) {
    return;
  }

  mTransform->addTranslation(translation);
  mIsModified = true;
}

void GameObject::setName(const std::string& name) { mName = name; }
void GameObject::setHealth(int health) {
  mHealth = health;
  mIsModified = true;
}

int GameObject::getPlayerId() {
  int id = getName()[NAME_LEN - 1] - '0';
  return id;
}

Transform* GameObject::getTransform() { return mTransform; }
std::string GameObject::getName() { return mName; }
int GameObject::getHealth() { return mHealth; }
ObjectType GameObject::getObjectType() { return mObjectType; }

std::string GameObject::makeName(std::string prefix, int count) {
  std::string name(prefix);

  // If the enemysSpawned is more than 4 digits, reset it
  if ((count / 1000) != 0) {
    // no need to update prefix
  } else if ((count / 100) != 0) {
    name += "0";
  } else if ((count / 10) != 0) {
    name += "00";
  } else {
    name += "000";
  }

  name += std::to_string(count);

  return name;
}

bool GameObject::isDefault() const {
  return mObjectType == ObjectType::Default;
}
bool GameObject::isPlayer() const { return mObjectType == ObjectType::Player; }
bool GameObject::isEnemy() const { return mObjectType == ObjectType::Enemy; }
bool GameObject::isTower() const { return mObjectType == ObjectType::Tower; }
bool GameObject::isCloud() const { return mObjectType == ObjectType::Cloud; }
bool GameObject::isPickup() const { return mObjectType == ObjectType::Pickup; }
bool GameObject::isProjectile() const {
  return mObjectType == ObjectType::Projectile;
}

bool GameObject::isDead() const { return mHealth <= 0; }

bool GameObject::shouldDelete() const {
  return !isPlayer() && !isTower() && !mShouldRender;
}

bool GameObject::hasHealth() const {
  return (isPlayer() || isEnemy() || isTower()) && !isDead();
}

bool GameObject::isModifiable() const {
  return isPlayer() || isEnemy() || isProjectile() || isTower() || isCloud() ||
         isPickup();
}
