#include "Tower.h"

#include "GameLogicServer.h"

int Tower::mTowerSpawned = 0;

void Tower::update() {
  // TODO: add healing feature?
  // mIsModified = true;
}

void Tower::spawn(int count) {
  GameLogicServer* logicServer = GameLogicServer::getLogicServer();
  for (int i = 1; i <= count; i++) {
    // TODO: spawn based on scene.json
    Tower* tower =
        new Tower(new Transform(glm::vec3(5 + 5 * i, 0, 0), glm::vec3(0),
                                glm::vec3(1), glm::vec3(0.5)),
                  (char*)makeName().c_str(), 100);

    logicServer->addGameObject(tower);
  }
}

std::string Tower::makeName() {
  return GameObject::makeName("tower", mTowerSpawned++);
}
