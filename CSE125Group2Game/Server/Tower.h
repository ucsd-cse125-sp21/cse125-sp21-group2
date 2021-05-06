#pragma once
#include "GameObject.h"

class Tower : public GameObject {
public:
    static int mTowerSpawned;

    Tower(Transform* transform, char* name, int health)
        : GameObject(transform, name, health, ObjectType::Tower) {}

    void update();

    static void spawn(int count);



protected:
    static std::string makeName();

};