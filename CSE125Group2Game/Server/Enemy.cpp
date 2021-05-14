#include "Enemy.h"

#include <glm/gtx/euler_angles.hpp>

#include "GameLogicServer.h"

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

  for (float i = 0; i < 360; i += 22.5) {
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
