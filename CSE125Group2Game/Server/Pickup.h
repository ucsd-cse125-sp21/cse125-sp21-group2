#pragma once
#include "GameObject.h"

#define NUM_PICKUPS 2
#define PICKUP_LENGTH 15000
enum class PickupType { None, DamageBoost, SpeedBoost };

class Pickup : public GameObject {
 public:
  static int pickupCount;

  Pickup(Transform* transform, PickupType type);

  static void spawnPickup(Transform* transform, PickupType type);
  // Spawn a random pickup
  static void spawnPickup(Transform* transform);

  PickupType mPickupType;
  std::string makeName();

  static bool isNone(PickupType pickupType);

  unsigned long mPickupDeleteTime;

  void update();
};
