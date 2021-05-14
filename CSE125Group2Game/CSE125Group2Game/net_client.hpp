#include <olc_net.h>

#include <iostream>
#include <set>

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

  void ClientMessageSend(bool* keysPressed) {
    olc::net::message<CustomMsgTypes> msg;
    msg.header.id = CustomMsgTypes::ClientMessage;

    int sum = 0;
    for (int i = 0; i < NUM_KEYS; i++) {
      sum += keysPressed[i];
      msg << keysPressed[i];
    }

    if (sum) {
      Send(msg);
    }
  }

  void Init(const std::string& address, int port) {
    this->Connect(address, port);
  }

  bool Update(bool* keysPressed) {
    // Added
    this->ClientMessageSend(keysPressed);

    if (this->IsConnected()) {
      // std::set<std::string> updatedObjects;

      while (!this->Incoming().empty()) {
        auto msg = this->Incoming().pop_front().msg;
        // auto msg = this->Incoming()
        //               .pop_back()
        //               .msg;  // if correctness is wrong, comment this out

        switch (msg.header.id) {
          case CustomMsgTypes::ServerAccept: {
            // Server has responded to a ping request
            std::cout << "Server Accepted Connection\n";

            // Get client id from message
            int clientId;
            msg >> clientId;

            GameManager::getManager()->mClientId = clientId;

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

            // std::cout << "Received message from server!" << std::endl;

            char* data = (char*)malloc(MESSAGE_SIZE);

            for (int i = 0; i < MESSAGE_SIZE; i++) {
              data[i] = msg.body[i];
            }

            GameObject* obj = GameManager::getManager()->unmarshalInfo(data);

            GameManager::getManager()->UpdateObject(obj);

            free(data);
            delete obj;
          } break;
        }
      }
    } else {
      CRASH("Server down!");
    }

    return false;
  }
};
