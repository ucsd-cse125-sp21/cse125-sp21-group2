#pragma once
#include "GameObject.h"

class Enemy : public GameObject {
  Enemy(Transform* transform, char* name, int health);
};
