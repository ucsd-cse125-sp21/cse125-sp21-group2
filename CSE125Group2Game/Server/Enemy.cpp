#include "Enemy.h"

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
    return;
  }

  mMoveDirection =
      glm::normalize(nearestPlayer->getTransform()->getTranslation() -
                     mTransform->getTranslation());

  mTransform->addTranslation(mMoveDirection * mMoveSpeed);
  // std::cout << mTransform->getTranslation().x << std::endl;
}

GameObject* Enemy::GetNearestPlayer() {
  GameLogicServer* logicServer = GameLogicServer::getLogicServer();

  float minDist = FLT_MAX;
  int worldIndex = 0;
  /*

 
  for (int i = 0; i < MAX_PLAYERS; i++) {
    if (logicServer->players[i] == NULL) {
      continue;
    }

    glm::vec3 playerPos =
        logicServer->players[i]->getTransform()->getTranslation();

    float distance = glm::length(playerPos - mTransform->getTranslation());

    if (distance < minDist) {
      minDist = distance;
      playerIndex = i;
    }
  } */

  auto mWorld = logicServer->getWorld();

  for (int i = 0; i < mWorld.size(); i++) {
      if (mWorld[i]->isPlayer() || mWorld[i]->isTower()) {
          if (mWorld[i] == NULL) {
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
