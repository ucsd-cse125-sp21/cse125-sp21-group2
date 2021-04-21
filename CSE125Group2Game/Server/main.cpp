#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <iostream>
#include <thread>

#include "CustomServer.h"
#include "GameLogicServer.h"

using namespace std;
int main() {
  DWORD before, after, diff;

  CustomServer *netServer = CustomServer::GetCustomServer();
  netServer->Start();

  GameLogicServer *logicServer = GameLogicServer::getLogicServer();

  logicServer->PrintWorld();

  std::thread netServerThread(&(CustomServer::Update), netServer, -1, true);

  // Write
  auto in = [](msd::channel<char *> &ch, CustomServer *netServer) {
    while (true) {
      // Read from channel and populate message
      for (char *out : ch) {
        olc::net::message<CustomMsgTypes> msg;
        msg.header.id = CustomMsgTypes::ServerMessage;

        // Byte by byte, populate message
        for (int i = 0; i < MESSAGE_SIZE; i++) {
          std::cout << out[i];

          msg << out[i];
        }

        std::cout << std::endl
                  << "Message size: " << msg.body.size() << std::endl;

        netServer->MessageAllClients(msg);
      }
    }
  };

  // Spawn thread to sendMessages
  std::thread sendMessages =
      std::thread{in, std::ref(logicServer->mSendingBuffer), netServer};

  // Runs at tickrate and performs game logic
  while (1) {
    before = GetTickCount();
    logicServer->Update();
    after = GetTickCount();

    diff = after - before;

    if (logicServer->GetServerTick() >
        diff) {  // need to ensure that server tick is big enough
      Sleep(logicServer->GetServerTick() - diff);
    }
  }

  return 0;
}
