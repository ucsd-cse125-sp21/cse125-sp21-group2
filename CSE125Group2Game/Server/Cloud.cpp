#include "Cloud.h"

#include "GameLogicServer.h"

Cloud::Cloud(Transform* transform, std::string name, int health)
    : Moveable(transform, name, health, ObjectType::Cloud) {}

void Cloud::update() {
  glm::vec3 rotate(0.0125);
  move(rotate);
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
