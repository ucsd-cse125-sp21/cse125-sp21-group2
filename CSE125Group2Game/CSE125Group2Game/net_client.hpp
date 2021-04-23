﻿#include <olc_net.h>

#include <iostream>

#include "GameObject.h"

class CustomClient : public olc::net::client_interface<CustomMsgTypes> {
 public:
  void PingServer() {
    olc::net::message<CustomMsgTypes> msg;
    msg.header.id = CustomMsgTypes::ServerPing;

    // Caution with this...
    std::chrono::system_clock::time_point timeNow =
        std::chrono::system_clock::now();

    Send(msg);
  }

  void MessageAll(bool* keysPressed) {
    olc::net::message<CustomMsgTypes> msg;
    msg.header.id = CustomMsgTypes::MessageAll;

    GameObject testObj(new Transform(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0),
                                     glm::vec3(1, 1, 1)),
                       (char*)"test", 12);

    for (int i = 0; i < 4; i++) {
      msg << keysPressed[i];
    }

    Send(msg);
  }

  void Init(const std::string& address, int port) {
    this->Connect(address, port);
  }

  bool Update(bool* keysPressed) {
    bool key[3] = {false, false, false};
    bool old_key[3] = {false, false, false};
    if (GetForegroundWindow() == GetConsoleWindow()) {
      key[0] = GetAsyncKeyState('1') & 0x8000;
      key[1] = GetAsyncKeyState('2') & 0x8000;
      key[2] = GetAsyncKeyState('3') & 0x8000;
    }

    // Added
    this->MessageAll(keysPressed);

    if (key[0] && !old_key[0]) this->PingServer();
    if (key[1] && !old_key[1]) this->MessageAll(keysPressed);
    if (key[2] && !old_key[2]) return true;

    for (int i = 0; i < 3; i++) old_key[i] = key[i];

    if (this->IsConnected()) {
      if (!this->Incoming().empty()) {
        auto msg = this->Incoming().pop_front().msg;

        switch (msg.header.id) {
          case CustomMsgTypes::ServerAccept: {
            // Server has responded to a ping request
            std::cout << "Server Accepted Connection\n";

            // Get client id from message
            int clientId;
            msg >> clientId;

            GameManager::getManager()->AddPlayer(clientId);

          } break;

          case CustomMsgTypes::ServerPing: {
            // Server has responded to a ping request
            std::chrono::system_clock::time_point timeNow =
                std::chrono::system_clock::now();
            std::chrono::system_clock::time_point timeThen;
            msg >> timeThen;
            std::cout
                << "Ping: "
                << std::chrono::duration<double>(timeNow - timeThen).count()
                << "\n";
          } break;

          case CustomMsgTypes::ServerMessage: {
            if (msg.body.size() != MESSAGE_SIZE) {
              std::cerr << "Error: Message body size of " << msg.body.size()
                        << " is incorrect in net_client update. Expected "
                        << MESSAGE_SIZE << std::endl;
              return false;
            }

            char* data = (char*)malloc(MESSAGE_SIZE);

            for (int i = 0; i < MESSAGE_SIZE; i++) {
              data[i] = msg.body[i];
            }

            GameObject* obj = GameManager::getManager()->Unmarshal(data);
            GameManager::getManager()->UpdateObject(obj);
            delete obj;

          } break;
        }
      }
    } else {
      std::cout << "Server Down\n";
      return true;
    }

    return false;
  }
};
