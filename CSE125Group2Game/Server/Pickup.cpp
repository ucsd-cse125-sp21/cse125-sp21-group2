#include "Pickup.h"

#include "GameLogicServer.h"

int Pickup::pickupCount = 0;

Pickup::Pickup(Transform* transform, PickupType type)
    : GameObject(transform, makeName(), DEFAULT_HEALTH, ObjectType::Pickup),
      mPickupType(type) {}

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
