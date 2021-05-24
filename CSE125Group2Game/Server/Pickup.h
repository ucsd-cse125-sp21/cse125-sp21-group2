#pragma once
#include "GameObject.h"

#define NUM_PICKUPS 1
enum class PickupType { None, DamageBoost };

class Pickup : public GameObject {
 public:
  static int pickupCount;

  Pickup(Transform* transform, PickupType type);

  static void spawnPickup(Transform* transform, PickupType type);

  PickupType mPickupType;
  std::string makeName();

  static bool isNone(PickupType pickupType);
};
