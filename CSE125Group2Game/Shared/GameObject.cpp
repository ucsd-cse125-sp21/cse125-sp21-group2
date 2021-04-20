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

void GameObject::setTransform(Transform* transform) { mTransform = transform; }
void GameObject::setName(char* name) { memcpy(mName, name, NAME_LEN); }
void GameObject::setHealth(int health) { mHealth = health; }

Transform* GameObject::getTransform() { return mTransform; }
char* GameObject::getName() { return mName; }
int GameObject::getHealth() { return mHealth; }
ObjectType GameObject::getObjectType() { return mObjectType; }

// TODO: Make sure to free msg
uint8_t* GameObject::serializeInfo() {
  /*
  // Double check sizeof(mHealth)
  int size = /*sizeof(Transform) +
  sizeof(char) * mName.length() + sizeof(mHealth) + sizeof(mObjectType);

  // memcpy(msg, mTransform, sizeof(Transform));
  char name[] = {'t', 'e', 's', 't', '\0'};
  memcpy(msg, name, 5);
  memcpy(msg + 5, &mHealth, sizeof(int));
  */

  uint8_t* msg = (uint8_t*)malloc(10);

  return msg;
}
