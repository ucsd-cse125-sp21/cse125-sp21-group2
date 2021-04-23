#include "GameObject.h"

GameObject::GameObject(Transform* transform, char* name, int health)
    : mTransform(transform), mHealth(health), mObjectType(ObjectType::Default) {
  memcpy(mName, name, NAME_LEN);
}

GameObject::GameObject(Transform* transform, char* name, int health,
                       ObjectType type)
    : mTransform(transform), mHealth(health), mObjectType(type) {
  memcpy(mName, name, NAME_LEN);
}

GameObject::~GameObject() {
  delete mTransform;
}

void GameObject::setTransform(Transform* transform) { mTransform = transform; }
void GameObject::setName(char* name) { memcpy(mName, name, NAME_LEN); }
void GameObject::setHealth(int health) { mHealth = health; }

Transform* GameObject::getTransform() { return mTransform; }
char* GameObject::getName() { return mName; }
int GameObject::getHealth() { return mHealth; }
ObjectType GameObject::getObjectType() { return mObjectType; }
