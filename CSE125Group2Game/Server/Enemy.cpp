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
    return;
  }

  // 1) get closest player and create vector A
  // 2) get forward vector B
  // (https://stackoverflow.com/questions/53608944/getting-a-forward-vector-from-rotation-and-position)
  // 3) dot the two to get rotation
  // (https://stackoverflow.com/questions/23692077/rotate-object-to-face-point)
  // 4) call move() to rotate on the y axis the rotation from 3
  // 5) call move() to rotate forward on the x axis speed degrees per tick

  float step = 22.5;

  glm::mat4 currentModel = mTransform->getModel();
  glm::vec3 currentPos = mTransform->getTranslation();

  float minLength = FLT_MAX;
  float angle;
  // optimization, if dot is negative only try obtuse angles
  for (float i = 0; i < 360; i += step) {
    // std::cout << minLength << " " << i << std::endl;

    // move towards
    glm::mat4 tmpPivotModel =
        mPivot->getModel() * glm::eulerAngleXYZ(.1f, glm::radians(i), 0.0f);
    glm::mat4 newModel = tmpPivotModel * mModelTransform->getModel();
    glm::vec3 newPos(newModel * glm::vec4(0, 0, 0, 1));

    // get length
    float towardsLength =
        glm::length(nearestPlayer->getTransform()->getTranslation() - newPos);
    std::cout << "Towards" << towardsLength << std::endl;
    std::cout << "Current min" << minLength << std::endl;

    if (towardsLength < minLength) {
      minLength = towardsLength;
      angle = glm::radians(i);
    }
  }

  move(glm::vec3(0.1f, angle, 0));

  /*std::cout << mTransform->getTranslation().x << " "
            << mTransform->getTranslation().y << " "
            << mTransform->getTranslation().z << std::endl;*/

  /*mMoveDirection =
      glm::normalize(nearestPlayer->getTransform()->getTranslation() -
                     mTransform->getTranslation());

  mTransform->addTranslation(mMoveDirection * mMoveSpeed);*/
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
