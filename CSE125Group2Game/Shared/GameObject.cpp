#include "GameObject.h"

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
