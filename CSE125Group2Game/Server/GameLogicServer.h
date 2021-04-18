#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include "ServerLoader.h"

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

 private:
  std::vector<GameObject*> mWorld;
  ServerLoader mScene;
  uint16_t mTick_ms;

  static GameLogicServer* mLogicServer;
};
