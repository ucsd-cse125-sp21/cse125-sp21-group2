#include "Pickup.h"

#include "GameLogicServer.h"

int Pickup::pickupCount = 0;

void Pickup::spawnPickup(Transform* transform, PickupType type) {
  Pickup* pickup = new Pickup(transform, type);

  GameLogicServer::getLogicServer()->addGameObject(pickup);
}
