#include "GameObject.h"

GameObject::GameObject(Transform* transform, std::string name, int health)
    : mTransform(transform), mName(name), mHealth(health) {}

void GameObject::setTransform(Transform* transform) { mTransform = transform; }
void GameObject::setName(std::string name) { mName = name; }
void GameObject::setHealth(int health) { mHealth = health; }

Transform* GameObject::getTransform() { return mTransform; }
std::string GameObject::getName() { return mName; }
int GameObject::getHealth() { return mHealth; }
