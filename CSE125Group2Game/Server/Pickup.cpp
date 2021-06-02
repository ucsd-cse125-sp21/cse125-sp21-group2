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
  pickup->mModifier = (int)type;

  GameLogicServer::getLogicServer()->addGameObject(pickup);
}

void Pickup::spawnPickup(Transform* transform) {
  int pickupIndex = (rand() % NUM_PICKUPS) + 1;  // Plus 1 to avoid none
  PickupType type = static_cast<PickupType>(pickupIndex);
  // type = PickupType::NoShooting;  // TODO: Comment this out for random
  // pickups!
  spawnPickup(transform, type);
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

  // Spin the pickup
  mTransform->addRotation(SPEED_MULTIPLIER * glm::vec3(2, 0, 0), true);
}
