#include "Cloud.h"

#include "GameLogicServer.h"

Cloud::Cloud(Transform* transform, std::string name, int health)
    : Moveable(transform, name, health, ObjectType::Cloud) {
  // TODO: rotate around y and one of x or z
  // mRotationAngle.x = ((rand() % 10) + 1.0) / 20000.0;
  mRotationAngle.x = 0;
  mRotationAngle.y = ((rand() % 10) + 1.0) / 20000.0;
  mRotationAngle.z = ((rand() % 10) + 1.0) / 20000.0;
  // mRotationAngle.z = 0;

  // if ((rand() % 2)) {
  //  mRotationAngle.x = -mRotationAngle.x;
  //}
  if ((rand() % 2)) {
    mRotationAngle.y = -mRotationAngle.y;
  }
  if ((rand() % 2)) {
    mRotationAngle.z = -mRotationAngle.z;
  }

  mRotationAngle = SPEED_MULTIPLIER * mRotationAngle;
}

void Cloud::update() {
  move(mRotationAngle);
  mIsModified = true;
}

void Cloud::spawn() {
  GameLogicServer* logicServer = GameLogicServer::getLogicServer();

  for (int i = 0; i < logicServer->mScene.mClouds.size(); i++) {
    Transform* transform = logicServer->mScene.mClouds[i]->getTransform();
    Cloud* cloud = new Cloud(
        new Transform(transform->getTranslation(), glm::vec3(0, 0, 90),
                      transform->getScale(), transform->getBBox()),
        logicServer->mScene.mClouds[i]->getName(), DEFAULT_HEALTH);

    logicServer->addGameObject(cloud);
  }
}

bool Cloud::shouldNotCollide(GameObject* obj) { return true; }
