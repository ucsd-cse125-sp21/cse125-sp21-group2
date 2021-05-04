#include "GameObject.h"

#include <iostream>

GameObject::GameObject(Transform* transform, char* name, int health,
                       glm::vec3 forwardVector)
    : mTransform(transform),
      mHealth(health),
      mObjectType(ObjectType::Default),
      mForwardVector(forwardVector) {
  memcpy(mName, name, NAME_LEN);
}

GameObject::GameObject(Transform* transform, char* name, int health,
                       ObjectType type, glm::vec3 forwardVector)
    : mTransform(transform),
      mHealth(health),
      mObjectType(type),
      mForwardVector(forwardVector) {
  memcpy(mName, name, NAME_LEN);
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

void GameObject::setName(char* name) { memcpy(mName, name, NAME_LEN); }
void GameObject::setHealth(int health) {
  mHealth = health;
  mIsModified = true;
}

Transform* GameObject::getTransform() { return mTransform; }
char* GameObject::getName() { return mName; }
int GameObject::getHealth() { return mHealth; }
ObjectType GameObject::getObjectType() { return mObjectType; }
glm::vec3 GameObject::getForwardVector() { return mForwardVector; }

void GameObject::setForwardVector(glm::vec3 forwardVector) {
  // TODO: is this right?
  mForwardVector.x = forwardVector.x;
  mForwardVector.y = forwardVector.y;
  mForwardVector.z = forwardVector.z;
}

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
  std::cout << "New Enemy Name: " << name << std::endl;

  return name;
}

bool GameObject::isDefault() { return mObjectType == ObjectType::Default; }
bool GameObject::isPlayer() { return mObjectType == ObjectType::Player; }
bool GameObject::isEnemy() { return mObjectType == ObjectType::Enemy; }
bool GameObject::isProjectile() {
  return mObjectType == ObjectType::Projectile;
}
