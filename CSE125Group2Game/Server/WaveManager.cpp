#include "WaveManager.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <iostream>

#include "GameLogicServer.h"

WaveManager* WaveManager::mWaveManager;
int Enemy::enemysSpawned = 0;

WaveManager::WaveManager() {
  // Spawn one enemy every
  mSpawnSpeed = 1.0f * 1000;
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
  if (!fullWaveSpawned() && GetTickCount() - mTimeOfLastSpawn > mSpawnSpeed) {
    spawnEnemy();
  }
}

void WaveManager::startWave() {
  // TODO????: random enemy spawns would be dope
  for (int i = 0; i < mMaxWaveSize; i++) {
    Enemy* enemy =
        new Enemy(new Transform(glm::vec3(0, -RADIUS, 0), glm::vec3(0),
                                glm::vec3(.25), glm::vec3(0.5)),
                  Enemy::makeName(), 10);

    mWaveEnemies.push_back(enemy);
  }

  mNextEnemyIndex = 0;
  mEnemiesSpawnedInWave = 0;
  // std::cout << "starting wave, mNextEnemyIndex: " << mNextEnemyIndex
  //          << "mWaveEnemies.size(): " << mWaveEnemies.size() << std::endl;
}

void WaveManager::spawnEnemy() {
  // Start the logic for the enemy
  GameLogicServer::getLogicServer()->addGameObject(
      mWaveEnemies[mNextEnemyIndex]);

  mTimeOfLastSpawn = GetTickCount();
  mNextEnemyIndex++;
  mEnemiesSpawnedInWave++;

  // std::cout << "adding player, mNextEnemyIndex: " << mNextEnemyIndex
  //          << "mWaveEnemies.size(): " << mWaveEnemies.size() << std::endl;
}

void WaveManager::removeEnemy(Enemy* enemy) {
  // Loop through all enemies and delete the one with the same name
  for (int i = 0; i < mWaveEnemies.size(); i++) {
    if (enemy->getName() == mWaveEnemies[i]->getName()) {
      mWaveEnemies.erase(mWaveEnemies.begin() + i);
      mNextEnemyIndex--;
      // std::cout << "deleting player, mNextEnemyIndex: " << mNextEnemyIndex
      //          << "mWaveEnemies.size(): " << mWaveEnemies.size() <<
      //          std::endl;
      return;
    }
  }
}

// std::string WaveManager::makeName() {
//  // std::string name;
//
//  // If the enemysSpawned is more than 4 digits, reset it
//  if (Enemy::enemysSpawned >= 10000) {
//    Enemy::enemysSpawned = 0;
//  }
//
//  std::string name = GameObject::makeName("enem", Enemy::enemysSpawned);
//  Enemy::enemysSpawned++;
//  return name;
//}

bool WaveManager::isWaveComplete() { return mWaveEnemies.size() == 0; }
bool WaveManager::fullWaveSpawned() {
  return mEnemiesSpawnedInWave >= mMaxWaveSize;
}
