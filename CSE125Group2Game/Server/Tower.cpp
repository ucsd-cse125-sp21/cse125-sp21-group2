#include "Tower.h"

#include "GameLogicServer.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

int Tower::mTowerSpawned = 0;

void Tower::update() {
  // Do nothing if at full health
  if (mHealth >= TOWER_HEALTH ||
      GetTickCount() - mLastHeal < TOWER_HEAL_RATE_MS) {
    return;
  }

  mLastHeal = GetTickCount();
  mHealth += TOWER_HEAL_AMT;

  // Ensure health never goes above max
  if (mHealth > TOWER_HEALTH) {
    mHealth = TOWER_HEALTH;
  }

  std::cout << "Tower health set to " << mHealth << std::endl;
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

void Tower::setHealth(int amt) {
  // reset timer if taking damage
  if (amt < mHealth) {
    mLastHeal = GetTickCount();
  }

  GameObject::setHealth(amt);
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
