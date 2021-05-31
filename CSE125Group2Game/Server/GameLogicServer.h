#pragma once

#include <Windows.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#include "Octree.h"
#include "Player.h"
#include "PriorityMutex.h"
#include "ServerLoader.h"
#include "msd/channel.hpp"

class Tower;
class Cloud;

/*
  http://www.cplusplus.com/reference/mutex/unique_lock/#:~:text=A%20unique%20lock%20is%20an%20object%20that%20manages,The%20object%20supports%20both%20states%3A%20locked%20and%20unlocked.
  PriorityMutex mtx;
  mtx.lock(true);  // acquire 'priority lock'
  std::unique_lock<decltype(mtx)> lk(mtx, std::adopt_lock);
  lk.unlock();*/

#define MAX_PLAYERS 4
#define MIN_X 0
#define MIN_Y 1
#define MIN_Z 2
#define MAX_X 3
#define MAX_Y 4
#define MAX_Z 5
#define MIN_PLAYERS 2

class GameLogicServer {
 public:
  GameLogicServer(std::vector<GameObject*> world, ServerLoader scene,
                  uint16_t tick_ms, bool friendlyFire);

  void update();
  static GameLogicServer* getLogicServer();

  uint16_t getServerTick();
  std::vector<GameObject*> getWorld();
  ServerLoader getScene();

  void printWorld();

  void updateKeyPress(int keyID, int playerID);

  void addGameObject(GameObject* obj);

  GameObject* getCollidingObject(GameObject* obj);
  void spawnPlayerExplosion(Player* player);

  void sendInfo();
  void sendWaveTimer(int seconds);
  bool isGameOver();

  msd::channel<char*> mSendingBuffer;  // Queue for storing events to send
  std::vector<char*> mTestBuffer;

  Player* players[MAX_PLAYERS];
  std::vector<Tower*> mTowers;
  std::vector<Cloud*> mClouds;
  ServerLoader mScene;
  PriorityMutex mMtx;
  float mLastTime = 0;
  float mDeltaTime;
  bool startGame = false;
  bool playerReady[MAX_PLAYERS];
  bool playersReady();
  int numReadyPlayers;
  bool mFriendlyFire;

 private:
  void resetKeyPresses();
  char* marshalInfo(GameObject* obj);
  void updatePlayers();
  void updatePickups();
  void handlePlayerCollision(int playerIndex);
  void updateEnemies();
  void updateTowers();
  void updateClouds();
  void updateProjectiles();
  std::vector<glm::vec3> getCorners(GameObject* obj);
  std::vector<float> getMinMax(GameObject* obj);
  void deleteObject(int worldIndex);
  void printKeyPresses();
  int getVerticalInput(int playerId);
  int getHorizontalInput(int playerId);
  void movePlayerToBoundary(Player* player);
  void updatePlayerPosition(int playerId);
  void restartGame();
  void sendEndGame();

  std::vector<GameObject*> mWorld;
  Octree mOctree;
  uint16_t mTick_ms;

  DWORD mGameStartTick;
  bool mPostGameInfoSent;

  std::vector<bool*> mKeyPresses;  // Queue for storing events before each
                                   // tick for each player

  static GameLogicServer* mLogicServer;
};
