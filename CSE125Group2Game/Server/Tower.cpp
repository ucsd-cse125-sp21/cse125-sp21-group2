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
    Tower* tower =
        new Tower(logicServer->mScene.mTowers[i]->getTransform(),
                  logicServer->mScene.mTowers[i]->getName(), TOWER_HEALTH);

    logicServer->addGameObject(tower);
  }
}

std::string Tower::makeName() {
  return GameObject::makeName("tower", mTowerSpawned++);
}

bool Tower::shouldNotCollide(GameObject* obj) {
  // TODO: Perhaps would be worth making more complex... enemies with
  // projectiles???? interesting question
  return GameObject::shouldNotCollide(obj) ||  // Call super method
         obj->isPlayer();
}
