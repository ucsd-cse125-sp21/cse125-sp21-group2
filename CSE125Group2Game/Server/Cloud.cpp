#include "Cloud.h"

#include "GameLogicServer.h"

Cloud::Cloud(Transform* transform, std::string name, int health)
    : Moveable(transform, name, health, ObjectType::Cloud) {
  mRotationAngle.x = ((rand() % 10) + 1.0) / 1500.0;
  mRotationAngle.y = ((rand() % 10) + 1.0) / 1500.0;
  mRotationAngle.z = ((rand() % 10) + 1.0) / 1500.0;

  if ((rand() % 2)) {
    mRotationAngle.x = -mRotationAngle.x;
  }
  if ((rand() % 2)) {
    mRotationAngle.y = -mRotationAngle.y;
  }
  if ((rand() % 2)) {
    mRotationAngle.z = -mRotationAngle.z;
  }
}

void Cloud::update() {
  move(mRotationAngle);
  mIsModified = true;
}

void Cloud::spawn() {
  GameLogicServer* logicServer = GameLogicServer::getLogicServer();

  for (int i = 0; i < logicServer->mScene.mClouds.size(); i++) {
    Cloud* tower =
        new Cloud(logicServer->mScene.mClouds[i]->getTransform(),
                  logicServer->mScene.mClouds[i]->getName(), DEFAULT_HEALTH);

    logicServer->addGameObject(tower);
  }
}

bool Cloud::shouldNotCollide(GameObject* obj) { return true; }
