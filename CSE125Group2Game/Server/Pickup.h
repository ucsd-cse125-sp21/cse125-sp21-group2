#pragma once
#include "GameObject.h"

#define NUM_PICKUPS 1
enum class PickupType { DamageBoost };

class Pickup : GameObject {
 public:
  static int pickupCount;

  Pickup(Transform* transform, PickupType type)
      : GameObject(transform, GameObject::makeName("pick", pickupCount++), DEFAULT_HEALTH,
                   ObjectType::Pickup),
        mPickupType(type) {}

  static void spawnPickup(Transform* transform, PickupType type);

  PickupType mPickupType;
};
