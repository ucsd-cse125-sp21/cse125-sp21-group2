#pragma once

#include "Enemy.h"

class WaveManager {
 public:
  WaveManager();

  // Check if wave is complete
  static WaveManager* getWaveManager();

  void update();

  void removeEnemy(Enemy* enemy);

 private:
  void startWave();
  bool fullWaveSpawned();
  bool isWaveComplete();
  void spawnEnemy();
  std::string makeName();
  static WaveManager* mWaveManager;

  bool mInWave = false;
  // bool mFullWaveSpawned = false;
  int mEnemiesSpawnedInWave = 0;
  int mWavesCompleted = 0;
  int mMaxWaveSize = 1000;
  int mNextEnemyIndex = 0;

  unsigned long mTimeOfLastSpawn;
  unsigned long mSpawnSpeed;

  std::vector<Enemy*> mWaveEnemies;
};
