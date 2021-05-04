#include "Projectile.h"

#include "GameLogicServer.h"
int Projectile::mProjectilesSpawned = 0;
Projectile::Projectile(Transform* transform, char* name, int health,
                       glm::vec3 forwardVector)
    : GameObject(transform, name, health, ObjectType::Projectile,
                 forwardVector) {
  mProjectileNextTick = 0;
};

void Projectile::calculatePath() {
  // TODO: maybe make more complex idk
  mPath.push_back(mTransform->getTranslation());

  for (int i = 1; i < mProjectileMaxTicks; i++) {
    mPath.push_back(mPath[mPath.size() - 1] + glm::normalize(mForwardVector));
  }
};

void Projectile::spawnProjectile(glm::vec3 translation,
                                 glm::vec3 forwardVector) {
  // create projectile
  GameObject* projectile = new Projectile(
      new Transform(translation, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0),
                    glm::vec3(0.5, 0.5, 0.5)),
      (char*)makeName().c_str(), 15, forwardVector);
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

void Projectile::Update() {
  mIsModified = true;

  // projectile is dead
  if (mProjectileNextTick >= mProjectileMaxTicks) {
    // setHealth(0); - TODO add back in
    return;
  }

  mTransform->setTranslation(mPath[mProjectileNextTick++]);
}
