﻿#include "WaveManager.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <iostream>

#include "GameLogicServer.h"

WaveManager* WaveManager::mWaveManager;
int Enemy::enemysSpawned = 0;

WaveManager::WaveManager() {
  // Spawn one enemy every
  mSpawnSpeed = 1 * 1000;
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
    spawnEnemyInWave();
  }
}

void WaveManager::startWave() {
  for (int i = 0; i < mMaxWaveSize; i++) {
    Enemy* enemy = Enemy::spawnEnemy();

    mWaveEnemies.push_back(enemy);
  }

  mNextEnemyIndex = 0;
  mEnemiesSpawnedInWave = 0;
}

void WaveManager::spawnEnemyInWave() {
  // Start the logic for the enemy
  GameLogicServer::getLogicServer()->addGameObject(
      mWaveEnemies[mNextEnemyIndex]);

  mTimeOfLastSpawn = GetTickCount();
  mNextEnemyIndex++;
  mEnemiesSpawnedInWave++;
}

void WaveManager::removeEnemy(Enemy* enemy) {
  // Loop through all enemies and delete the one with the same name
  for (int i = 0; i < mWaveEnemies.size(); i++) {
    if (enemy->getName() == mWaveEnemies[i]->getName()) {
      mWaveEnemies.erase(mWaveEnemies.begin() + i);
      mNextEnemyIndex--;
      std::cout << "deleting player, mNextEnemyIndex: " << mNextEnemyIndex
                << "mWaveEnemies.size(): " << mWaveEnemies.size() << std::endl;
      return;
    }
  }
}

void WaveManager::reset() {
  mWavesCompleted = 0;
  mWaveEnemies.clear();
}
bool WaveManager::isWaveComplete() { return mWaveEnemies.size() == 0; }

bool WaveManager::fullWaveSpawned() {
  return mEnemiesSpawnedInWave >= mMaxWaveSize;
}
