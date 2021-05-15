#include "Tower.h"

#include "GameLogicServer.h"

int Tower::mTowerSpawned = 0;

void Tower::update() {
  // TODO: add healing feature?
  // mIsModified = true;
}

void Tower::spawn() {
  GameLogicServer* logicServer = GameLogicServer::getLogicServer();

  for (int i = 0; i < logicServer->mScene.mTowers.size(); i++) {
    // TODO: spawn based on scene.json
    Tower* tower = new Tower(logicServer->mScene.mTowers[i]->getTransform(),
                             logicServer->mScene.mTowers[i]->getName(), 100);

    logicServer->addGameObject(tower);
  }
}

std::string Tower::makeName() {
  return GameObject::makeName("tower", mTowerSpawned++);
}
