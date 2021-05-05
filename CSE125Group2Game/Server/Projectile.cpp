#include "Projectile.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "GameLogicServer.h"

int Projectile::mProjectilesSpawned = 0;
unsigned long Projectile::mTickLastSpawn = 0;  //[MAX_PLAYERS] = {0, 0, 0, 0};

Projectile::Projectile(Transform* transform, char* name, int health,
                       glm::vec3 forwardVector, GameObject* parent)
    : Moveable(transform, name, health, ObjectType::Projectile, forwardVector) {
  mProjectileNextTick = 0;
  mParent = parent;
};

void Projectile::calculatePath() {
  // TODO: maybe make more complex idk
  mPath.push_back(mTransform->getTranslation());

  for (int i = 1; i < mProjectileMaxTicks; i++) {
    mPath.push_back(mPath[mPath.size() - 1] + glm::normalize(mForwardVector));
  }
};

void Projectile::spawnProjectile(GameObject* parent) {
  if (GetTickCount() - Projectile::mTickLastSpawn < PROJ_SPAWN_RATE_MS) {
    return;
  }

  Projectile::mTickLastSpawn = GetTickCount();  // update last spawn time

  // create projectile
  GameObject* projectile = new Projectile(
      new Transform(parent->getTransform()->getTranslation(),
                    glm::vec3(0, 0, 0), glm::vec3(0.1, 0.1, 0.1),
                    glm::vec3(0.05, 0.05, 0.05)),
      (char*)makeName().c_str(), 15, parent->getForwardVector(), parent);
  // calculate path
  ((Projectile*)projectile)->calculatePath();
  // add to game world
  GameLogicServer::getLogicServer()->addGameObject(projectile);

  // std::cout << "Projecile spawned!";
}

// TODO: add method update every tick or whatever
std::vector<glm::vec3> Projectile::getPath() { return mPath; }

std::string Projectile::makeName() {
  return GameObject::makeName("proj", mProjectilesSpawned++);
}

void Projectile::update() {
  mIsModified = true;

  // projectile is dead
  if (mProjectileNextTick >= mProjectileMaxTicks) {
    setHealth(0);
    return;
  }

  mTransform->setTranslation(mPath[mProjectileNextTick++]);
}

bool Projectile::shouldNotCollide(GameObject* obj) {
  return GameObject::shouldNotCollide(obj) ||  // Call super method
         !(strncmp(obj->getName(), mParent->getName(), NAME_LEN)) ||
         obj->isProjectile() || obj->isPlayer();
  // TODO: Perhaps would be worth making more complex... enemies with
  // projectiles???? interesting question
}
