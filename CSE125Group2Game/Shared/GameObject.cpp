#include "GameObject.h"

#include <iostream>

GameObject::GameObject(Transform* transform, const std::string& name,
                       int health)
    : mTransform(transform),
      mHealth(health),
      mObjectType(ObjectType::Default),
      mName(name) {}

GameObject::GameObject(Transform* transform, const std::string& name,
                       int health, ObjectType type)
    : mTransform(transform), mHealth(health), mObjectType(type), mName(name) {}

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

bool GameObject::isDefault() { return mObjectType == ObjectType::Default; }
bool GameObject::isPlayer() { return mObjectType == ObjectType::Player; }
bool GameObject::isEnemy() { return mObjectType == ObjectType::Enemy; }
bool GameObject::isTower() { return mObjectType == ObjectType::Tower; }
bool GameObject::isProjectile() {
  return mObjectType == ObjectType::Projectile;
}

bool GameObject::isDead() { return mHealth <= 0; }

bool GameObject::shouldDelete() {
  return !isPlayer() && !isTower() && !mShouldRender;
}
