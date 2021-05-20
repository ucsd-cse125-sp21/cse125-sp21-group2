#pragma once

#include "Enemy.h"

#define WAVE_INTERMISSION 10000

class WaveManager {
 public:
  WaveManager();

  // Check if wave is complete
  static WaveManager* getWaveManager();

  void update();

  void removeEnemy(Enemy* enemy);

  void reset();

 private:
  void startWave();
  bool fullWaveSpawned();
  bool isWaveComplete();
  void spawnEnemyInWave();
  std::string makeName();
  static WaveManager* mWaveManager;

  int mEnemiesSpawnedInWave = 0;
  int mWavesCompleted = 0;
  int mMaxWaveSize = 10;
  int mNextEnemyIndex = 0;

  unsigned long mTimeOfLastSpawn;
  unsigned long mSpawnSpeed;

  std::vector<Enemy*> mWaveEnemies;
};
