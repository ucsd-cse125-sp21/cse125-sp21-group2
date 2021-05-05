#include "Enemy.h"

#include "GameLogicServer.h"

Enemy::Enemy(Transform* transform, char* name, int health,
             glm::vec3 fowardVector)
    : GameObject(transform, name, health, ObjectType::Enemy, fowardVector) {
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
  int playerIndex = 0;

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
  }

  return logicServer->players[playerIndex];
}
