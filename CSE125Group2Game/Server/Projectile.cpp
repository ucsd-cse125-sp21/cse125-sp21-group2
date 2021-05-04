#include "Projectile.h"

Projectile::Projectile(Transform* transform, char* name, int health,
                       glm::vec3 forwardVector)
    : GameObject(transform, name, health, ObjectType::Projectile,
                 forwardVector){};

void Projectile::calculatePath(){
    // TODO
};

void Projectile::spawnProjectile(glm::vec3 forwardVector) {
  // create projectile
  // GameObject* projectile = new Projectile(
  //    new Transform(forwardVector, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), ))
  // calculate path
  // add to game world
}

// TODO: add method update every tick or whatever
std::vector<glm::vec3> Projectile::getPath() { return mPath; }
