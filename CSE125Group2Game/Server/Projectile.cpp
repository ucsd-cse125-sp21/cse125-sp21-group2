#include "Projectile.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "GameLogicServer.h"

int Projectile::mProjectilesSpawned = 0;
std::unordered_map<std::string, unsigned long>
    Projectile::mTickLastSpawn;  // todo: might wanna change to hashmap to map
                                 // obj name to

Projectile::Projectile(Transform* transform, std::string name, int health,
                       GameObject* parent)
    : Moveable(transform, name, health, ObjectType::Projectile) {
  mProjectileNextTick = 0;
  mParent = parent;
};

void Projectile::spawnProjectile(GameObject* parent) {
  if (GetTickCount() - Projectile::mTickLastSpawn[parent->getName()] <
      PROJ_SPAWN_RATE_MS) {
    return;
  }

  Projectile::mTickLastSpawn[parent->getName()] =
      GetTickCount();  // update last spawn time

  glm::vec3 spawnPos = parent->getTransform()->getModelTranslation();

  // create projectile
  Projectile* projectile = new Projectile(
      new Transform(spawnPos, glm::vec3(0), glm::vec3(.5), glm::vec3(0.25)),
      Projectile::makeName(), 15, parent);

  // Update projectiles model/pivot
  projectile->mPivot->setModel(((Player*)parent)->mPivot->getModel());
  projectile->mModelTransform->setModel(
      ((Player*)parent)->mModelTransform->getModel() *
      glm::scale(glm::mat4(1), glm::vec3(.5)));

  // add to game world
  GameLogicServer::getLogicServer()->addGameObject(projectile);

  // std::cout << "Projecile spawned!";
}

std::string Projectile::makeName() {
  if (mProjectilesSpawned >= 10000) {
    mProjectilesSpawned = 0;
  }

  std::string name = GameObject::makeName("proj", mProjectilesSpawned);
  mProjectilesSpawned++;
  return name;
}

void Projectile::update() {
  mIsModified = true;

  // projectile is dead
  if (++mProjectileNextTick >= mProjectileMaxTicks) {
    setHealth(0);
    return;
  }

  move(glm::vec3(-2, 0, 0));

  std::cout << mTransform->getTranslation().x << " "
            << mTransform->getTranslation().y << " "
            << mTransform->getTranslation().z << std::endl;
}

bool Projectile::shouldNotCollide(GameObject* obj) {
  // TODO: Perhaps would be worth making more complex... enemies with
  // projectiles???? interesting question
  return GameObject::shouldNotCollide(obj) ||  // Call super method
         obj->getName() == mParent->getName() || obj->isProjectile() ||
         obj->isPlayer() || obj->isTower() || obj->isDefault();
}
