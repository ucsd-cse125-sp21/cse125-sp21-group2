#include "Pickup.h"

#include "GameLogicServer.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

int Pickup::pickupCount = 0;

Pickup::Pickup(Transform* transform, PickupType type)
    : GameObject(transform, makeName(), DEFAULT_HEALTH, ObjectType::Pickup),
      mPickupType(type) {
  mPickupDeleteTime = GetTickCount() + PICKUP_LENGTH;
}

void Pickup::spawnPickup(Transform* transform, PickupType type) {
  Pickup* pickup = new Pickup(transform, type);

  GameLogicServer::getLogicServer()->addGameObject(pickup);
}

std::string Pickup::makeName() {
  std::string name = GameObject::makeName("pick", pickupCount);

  pickupCount++;

  return name;
}
bool Pickup::isNone(PickupType pickupType) {
  return pickupType == PickupType::None;
}

void Pickup::update() {
  if (mPickupDeleteTime < GetTickCount()) {
    setHealth(0);
  }
}
