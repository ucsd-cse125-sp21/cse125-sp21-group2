﻿#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#include "PriorityMutex.h"
#include "ServerLoader.h"
#include "msd/channel.hpp"

#define MAX_PLAYERS 4

/*
  http://www.cplusplus.com/reference/mutex/unique_lock/#:~:text=A%20unique%20lock%20is%20an%20object%20that%20manages,The%20object%20supports%20both%20states%3A%20locked%20and%20unlocked.
  PriorityMutex mtx;
  mtx.lock(true);  // acquire 'priority lock'
  std::unique_lock<decltype(mtx)> lk(mtx, std::adopt_lock);
  lk.unlock();*/

// struct Vertices {
//  glm::vec3 bottomLeftBack;
//  glm::vec3 bottomRightBack;
//  glm::vec3 topLeftBack;
//  glm::vec3 topRightBack;
//  glm::vec3 bottomLeftFront;
//  glm::vec3 bottomRightFront;
//  glm::vec3 topLeftFront;
//  glm::vec3 topRightFront;
//};

#define MIN_X 0
#define MIN_Y 1
#define MIN_Z 2
#define MAX_X 3
#define MAX_Y 4
#define MAX_Z 5

class GameLogicServer {
 public:
  GameLogicServer(std::vector<GameObject*> world, ServerLoader scene,
                  uint16_t tick_ms);

  void Update();
  static GameLogicServer* getLogicServer();

  uint16_t GetServerTick();
  std::vector<GameObject*> GetWorld();
  ServerLoader GetScene();

  void PrintWorld();

  void UpdateKeyPress(int keyID, int playerID);

  void ResetKeyPresses();

  void AddGameObject(GameObject* obj);

  msd::channel<char*> mSendingBuffer;  // Queue for storing events to send
  std::vector<char*> mTestBuffer;

  GameObject* players[MAX_PLAYERS];

 private:
  void SendInfo();
  char* MarshalInfo(GameObject* obj);
  void MovePlayers();
  void HandlePlayerCollision(int playerIndex);
  void MoveEnemies();
  GameObject* DoesCollide(GameObject* obj);
  std::vector<glm::vec3> GetCorners(GameObject* obj);
  std::vector<float> GetMinMax(GameObject* obj);

  std::vector<GameObject*> mWorld;
  ServerLoader mScene;
  uint16_t mTick_ms;
  PriorityMutex mMtx;

  std::vector<bool*> mKeyPresses;  // Queue for storing events before each
                                   // tick for each player

  static GameLogicServer* mLogicServer;
};
