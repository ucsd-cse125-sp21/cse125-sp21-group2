#pragma once

#include "Enemy.h"

#define WAVE_INTERMISSION 5000

#define MAX_ENEMIES 50

class WaveManager {
 public:
  WaveManager();

  // Check if wave is complete
  static WaveManager* getWaveManager();

  void update();

  void removeEnemy(Enemy* enemy);

  void reset();
  int mWavesCompleted = 0;

 private:
  void startWave();
  bool fullWaveSpawned();
  bool isWaveComplete();
  void spawnEnemyInWave();
  std::string makeName();
  static WaveManager* mWaveManager;

  int mEnemiesSpawnedInWave = 0;
  int mMaxWaveSize = 40;
  int mNextEnemyIndex = 0;

  unsigned long mTimeOfLastSpawn;
  unsigned long mSpawnSpeed;

  std::vector<Enemy*> mWaveEnemies;
};
