#include "Enemy.h"

Enemy::Enemy(Transform* transform, char* name, int health)
    : GameObject(transform, name, health, ObjectType::Enemy) {}
