#include "WaveManager.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <iostream>

#include "GameLogicServer.h"

WaveManager* WaveManager::mWaveManager;
int Enemy::enemysSpawned = 0;

WaveManager::WaveManager() {
  // Spawn one enemy every
  mSpawnSpeed = 3 * 1000;
}

WaveManager* WaveManager::getWaveManager() {
  if (!mWaveManager) {
    mWaveManager = new WaveManager();
  }

  return mWaveManager;
}

void WaveManager::update() {
  if (isWaveComplete()) {
    startWave();
    return;
  }

  // Only spawn enemy once every mSpawnSpeed (3 seconds)
  if (mNextEnemyIndex < maxWaveSize &&
      GetTickCount() - mTimeOfLastSpawn > mSpawnSpeed) {
    spawnEnemy();
  }
}

void WaveManager::startWave() {
  // TODO????: random enemy spawns would be dope
  for (int i = 0; i < maxWaveSize; i++) {
    Enemy* enemy =
        new Enemy(new Transform(glm::vec3(-5 - i, 0, 0), glm::vec3(0, 0, 0),
                                glm::vec3(1, 1, 1), glm::vec3(0.5, 0.5, 0.5)),
                  (char*)makeName().c_str(), 10);

    mWaveEnemies.push_back(enemy);
  }

  mNextEnemyIndex = 0;
}

void WaveManager::spawnEnemy() {
  // Start the logic for the enemy
  GameLogicServer::getLogicServer()->addGameObject(
      mWaveEnemies[mNextEnemyIndex]);

  mTimeOfLastSpawn = GetTickCount();
  mNextEnemyIndex++;
}

void WaveManager::removeEnemy(Enemy* enemy) {
  // Loop through all enemies and delete the one with the same name
  for (int i = 0; i < mWaveEnemies.size(); i++) {
    if (!strncmp(enemy->getName(), mWaveEnemies[i]->getName(), NAME_LEN)) {
      mWaveEnemies.erase(mWaveEnemies.begin() + i);
      mNextEnemyIndex--;
      return;
    }
  }
}

std::string WaveManager::makeName() {
  std::string name;

  // If the enemysSpawned is more than 4 digits, reset it
  if (Enemy::enemysSpawned >= 10000) {
    Enemy::enemysSpawned = 0;
  }

  if ((Enemy::enemysSpawned / 1000) != 0) {
    std::string name = "enem";
  } else if ((Enemy::enemysSpawned / 100) == 0) {
    std::string name = "enem0";
  } else if ((Enemy::enemysSpawned / 10) == 0) {
    std::string name = "enem00";
  } else {
    std::string name = "enem000";
  }

  name += std::to_string(Enemy::enemysSpawned);
  std::cout << "New Enemy Name: " << name << std::endl;

  Enemy::enemysSpawned++;
  return name;
}

bool WaveManager::isWaveComplete() { return mWaveEnemies.size() == 0; }
