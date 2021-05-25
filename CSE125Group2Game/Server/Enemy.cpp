#include <glm/gtx/euler_angles.hpp>

#include "Enemy.h"
#include "GameLogicServer.h"
#include "Pickup.h"

#define STEP 12
#define MAX_ANGLE 360

Enemy::Enemy(Transform* transform, std::string name, int health)
    : Moveable(transform, name, health, ObjectType::Enemy) {
  mIsModified = true;
}

void Enemy::update() {
  if (--mTicksTillUpdate > 0) {
    mIsModified = false;
    return;
  }

  mIsModified = true;
  mTicksTillUpdate = mTicksPerUpdate;

  if (isDead()) {
    return;
  }

  GameObject* nearestPlayer = GetNearestPlayer();
  if (!nearestPlayer) {
    // Spin to "seek" out new player
    move(glm::vec3(0, rotationSpeed * 4, 0));
    return;
  }

  glm::mat4 currentPivotModel = mPivot->getModel();
  glm::mat4 currentActualModel = mTransform->getModel();

  float minDistance = FLT_MAX;
  float bestAngle;

  for (float i = 0; i < MAX_ANGLE; i += STEP) {
    move(glm::vec3(0, glm::radians(i), rotationSpeed));

    glm::vec3 newPos = mTransform->getModelTranslation();

    float distance = glm::length(
        newPos - nearestPlayer->getTransform()->getModelTranslation());

    if (distance < minDistance) {
      minDistance = distance;
      bestAngle = glm::radians(i);
    }

    mPivot->setModel(currentPivotModel);
    mTransform->setModel(currentActualModel);
  }

  move(glm::vec3(0, bestAngle, rotationSpeed));
}

GameObject* Enemy::GetNearestPlayer() {
  GameLogicServer* logicServer = GameLogicServer::getLogicServer();

  float minDist = FLT_MAX;
  int worldIndex = -1;

  auto mWorld = logicServer->getWorld();

  for (int i = 0; i < mWorld.size(); i++) {
    if (mWorld[i]->isPlayer() || mWorld[i]->isTower()) {
      // if (mWorld[i]->getHealth() <= 0) {
      if (mWorld[i]->isDead()) {
        continue;
      }
      glm::vec pos = mWorld[i]->getTransform()->getTranslation();
      float distance = glm::length(pos - mTransform->getTranslation());
      if (distance < minDist) {
        minDist = distance;
        worldIndex = i;
      }
    }
  }

  if (worldIndex < 0) {
    return nullptr;
  }

  return mWorld[worldIndex];
}

std::string Enemy::makeName() {
  if (enemysSpawned >= 10000) {
    enemysSpawned = 0;
  }

  std::string name = GameObject::makeName("enem", enemysSpawned);
  enemysSpawned++;
  return name;
}

Enemy* Enemy::spawnEnemy() {
  Enemy* enemy = new Enemy(
      new Transform(glm::vec3(0, RADIUS, 0),
                    glm::vec3(rand() % MAX_ANGLE, 0, rand() % MAX_ANGLE),
                    glm::vec3(.25), glm::vec3(5)),
      Enemy::makeName(), DEFAULT_HEALTH);
  enemy->move(glm::vec3(0));  // hack to fix world position

  unsigned int randomChance = rand() % 2;
  enemy->mSpawnPickup = randomChance == PICKUP_CHANCE;
  // enemy->mSpawnPickup = true;

  return enemy;
}

void Enemy::setHealth(int amt) {
  GameObject::setHealth(amt);

  // If enemy dies and should spawn a pickup, create one
  if (isDead() && mSpawnPickup) {
    Transform* pickupTransform =
        new Transform(mTransform->getTranslation(), mTransform->getRotation(),
                      mTransform->getScale(), mTransform->getBBoxLens());
    Pickup::spawnPickup(pickupTransform, PickupType::DamageBoost);
  }
}
bool Enemy::shouldNotCollide(GameObject* obj) {
  return GameObject::shouldNotCollide(obj) ||  // Call super method
         obj->isPickup();
}
