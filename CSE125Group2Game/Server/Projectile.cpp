#include "Projectile.h"

Projectile::Projectile(Transform* transform, char* name, int health,
                       glm::vec3 forwardVector)
    : GameObject(transform, name, health, ObjectType::Projectile,
                 forwardVector){};

void Projectile::calculatePath(){
    // TODO
};

std::vector<glm::vec3> Projectile::getPath() { return mPath; }
