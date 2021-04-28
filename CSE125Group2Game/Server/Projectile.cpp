#include "Projectile.h"

Projectile::Projectile(Transform* transform, char* name, int health)
    : GameObject(transform, name, health, ObjectType::Projectile){};

void Projectile::calculatePath(){
    // TODO
};

std::vector<glm::vec3> Projectile::getPath() { return mPath; }
