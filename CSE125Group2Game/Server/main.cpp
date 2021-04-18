#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <iostream>
#include <thread>

#include "GameLogicServer.h"
#include "CustomServer.h"

using namespace std;
int main() {
  DWORD before, after, diff;

  CustomServer *netServer = CustomServer::GetCustomServer();
  netServer->Start();

  GameLogicServer *logicServer = GameLogicServer::getLogicServer();

  logicServer->PrintWorld();

  thread netServerThread(&(CustomServer::Update), netServer, -1, true);

  // Runs at tickrate and performs game logic
  while (1) {
    before = GetTickCount();
    // server.Update(-1, true);
    after = GetTickCount();

    diff = after - before;

    if (logicServer->GetServerTick() >
        diff) {  // need to ensure that server tick is big enough
      Sleep(logicServer->GetServerTick() - diff);
    }
  }

  return 0;
}
