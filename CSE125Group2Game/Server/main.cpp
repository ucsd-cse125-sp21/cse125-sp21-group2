#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <iostream>
#include <thread>

#include "GameLogicServer.h"
#include "NetworkServer.h"

using namespace std;

NetworkServer *NetworkServer::mNetServer;

int main() {
  DWORD before, after, diff;

  NetworkServer *netServer = NetworkServer::GetNetworkServer();
  netServer->Start();

  GameLogicServer *logicServer = GameLogicServer::getLogicServer();

  logicServer->printWorld();

  std::thread netServerThread(&(NetworkServer::Update), netServer, -1, true);

  // Write
  /*auto sendMessage = [](msd::channel<char *> &ch, NetworkServer *netServer) {
    while (true) {
      // Read from channel and populate message
      for (int i = 0; i < ch.size(); i++) {
        char *out = nullptr;
        out << ch;

        if (!out) {
          continue;
        }

        olc::net::message<CustomMsgTypes> msg;
        msg.header.id = CustomMsgTypes::ServerMessage;

        // Byte by byte, populate message
        for (int i = 0; i < MESSAGE_SIZE; i++) {
          msg << out[i];
        }

        // std::cout << "Sending message to clients!" << std::endl;
        netServer->MessageAllClients(msg);

        // free(out);
      }
    }
  };

  // Spawn thread to sendMessages
  std::thread sendMessagesThread = std::thread{
      sendMessage, std::ref(logicServer->mSendingBuffer), netServer};*/

  // Runs at tickrate and performs game logic
  while (1) {
    before = GetTickCount();
    logicServer->update();
    after = GetTickCount();

    diff = after - before;

    for (int i = 0; i < logicServer->mTestBuffer.size(); i++) {
      char *data = logicServer->mTestBuffer[i];

      olc::net::message<CustomMsgTypes> msg;
      msg.header.id = CustomMsgTypes::ServerMessage;

      // Byte by byte, populate message
      for (int i = 0; i < MESSAGE_SIZE; i++) {
        msg << data[i];
      }

      netServer->MessageAllClients(msg);

      free(data);
    }

    logicServer->mTestBuffer.clear();

    if (logicServer->getServerTick() >
        diff) {  // need to ensure that server tick is big enough
      Sleep(logicServer->getServerTick() - diff);
    }
  }

  return 0;
}
