#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#include "PriorityMutex.h"
#include "ServerLoader.h"
#include "msd/channel.hpp"

/*
  //
  http://www.cplusplus.com/reference/mutex/unique_lock/#:~:text=A%20unique%20lock%20is%20an%20object%20that%20manages,The%20object%20supports%20both%20states%3A%20locked%20and%20unlocked.
  PriorityMutex mtx;
  mtx.lock(true);  // acquire 'priority lock'
  std::unique_lock<decltype(mtx)> lk(mtx, std::adopt_lock);
  lk.unlock();*/

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

  void UpdateKeyPress(int keyID);

  void ResetKeyPresses();

  void AddGameObject(GameObject* obj);

  msd::channel<char*> mSendingBuffer;  // Queue for storing events to send

 private:
  void SendInfo();
  char* MarshalInfo(GameObject* obj);

  std::vector<GameObject*> mWorld;
  ServerLoader mScene;
  uint16_t mTick_ms;
  PriorityMutex mMtx;

  bool mKeyPresses[NUM_KEYS];  // Queue for storing events before each tick

  static GameLogicServer* mLogicServer;
};
