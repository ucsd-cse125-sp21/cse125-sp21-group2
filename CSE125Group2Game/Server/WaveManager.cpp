#include "WaveManager.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <iostream>

#include "GameLogicServer.h"

WaveManager* WaveManager::mWaveManager;
int Enemy::enemysSpawned = 0;
void sendWaveTimer(int seconds, int wavesCompleted, int numEnemies,
                   int maxEnemyHealth);

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

  unsigned long currTick = GetTickCount();

  // Only spawn enemy once every mSpawnSpeed (3 seconds)
  if (!fullWaveSpawned() && currTick > mSpawnSpeed + mTimeOfLastSpawn) {
    spawnEnemyInWave();
  }

  // No enemies created yet, waiting for wave to start
  if (mEnemiesSpawnedInWave == 0) {
    unsigned long msLeftToWave = mTimeOfLastSpawn + mSpawnSpeed - currTick;

    int secondsLeftToWave = msLeftToWave / 1000;

    sendWaveTimer(secondsLeftToWave, mWavesCompleted, mMaxWaveSize,
                  Enemy::currHealth);
  }
}

void WaveManager::startWave() {
  mWavesCompleted++;
  // Make game harder as time goes on
  if (mMaxWaveSize < MAX_ENEMIES) {
    mMaxWaveSize += mWavesCompleted;
  }

  if (Enemy::currHealth < MAX_ENEMY_HEALTH) {
    Enemy::currHealth += (2 * mWavesCompleted);
  }

  for (int i = 0; i < mMaxWaveSize; i++) {
    Enemy* enemy = Enemy::spawnEnemy();

    mWaveEnemies.push_back(enemy);
  }

  mNextEnemyIndex = 0;
  mEnemiesSpawnedInWave = 0;
  mTimeOfLastSpawn =
      GetTickCount() + WAVE_INTERMISSION;  // so that we have some time at the
                                           // beginning without enemies
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

      return;
    }
  }
}

void WaveManager::reset() {
  mWavesCompleted = 0;
  mWaveEnemies.clear();
  mMaxWaveSize = 5;

  Enemy::currHealth = DEFAULT_HEALTH;
}
bool WaveManager::isWaveComplete() { return mWaveEnemies.size() == 0; }

bool WaveManager::fullWaveSpawned() {
  return mEnemiesSpawnedInWave >= mMaxWaveSize;
}
