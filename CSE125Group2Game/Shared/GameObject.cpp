#include "GameObject.h"

GameObject::GameObject(Transform* transform, std::string name, int health)
    : mTransform(transform),
      mName(name),
      mHealth(health),
      mObjectType(ObjectType::Default) {}

void GameObject::setTransform(Transform* transform) { mTransform = transform; }
void GameObject::setName(std::string name) { mName = name; }
void GameObject::setHealth(int health) { mHealth = health; }

Transform* GameObject::getTransform() { return mTransform; }
std::string GameObject::getName() { return mName; }
int GameObject::getHealth() { return mHealth; }
ObjectType GameObject::getObjectType() { return mObjectType; }

// TODO: Make sure to free msg
uint8_t* GameObject::serializeInfo() {
  // Double check sizeof(mHealth)
  int size = /*sizeof(Transform) +*/ sizeof(char) * mName.length() +
             sizeof(mHealth) + sizeof(mObjectType);

  uint8_t* msg = (uint8_t*)malloc(10);

  // memcpy(msg, mTransform, sizeof(Transform));
  char name[] = {'t', 'e', 's', 't', '\0'};
  memcpy(msg, name, 5);
  memcpy(msg + 5, &mHealth, sizeof(int));

  return msg;
}
