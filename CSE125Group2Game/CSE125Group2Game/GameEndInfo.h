#pragma once
#include <vector>

class GameEndInfo {
 public:
  std::vector<int> mEnemiesKilledPerPlayer;
  std::vector<int> mNumRespawnedPerPlayer;  // AKA NumDied
  unsigned long timeEllapsed;
  int highScore;
  int totalEnemiesKilled;
  int mvpPlayerID;
  int numPlayers;
};
