#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <fstream>
#include <iostream>
#include <string>

#include "olc_net.h"

class CustomServer : public olc::net::server_interface<CustomMsgTypes> {
 public:
  CustomServer(uint16_t nPort, uint16_t server_tick_ms)
      : olc::net::server_interface<CustomMsgTypes>(nPort) {
    tick_ms = server_tick_ms;
  }

  DWORD GetServerTick() { return tick_ms; }

 protected:
  virtual bool OnClientConnect(
      std::shared_ptr<olc::net::connection<CustomMsgTypes>> client) {
    olc::net::message<CustomMsgTypes> msg;
    msg.header.id = CustomMsgTypes::ServerAccept;
    client->Send(msg);
    return true;
  }

  // Called when a client appears to have disconnected
  virtual void OnClientDisconnect(
      std::shared_ptr<olc::net::connection<CustomMsgTypes>> client) {
    std::cout << "Removing client [" << client->GetID() << "]\n";
  }

  // Called when a message arrives
  virtual void OnMessage(
      std::shared_ptr<olc::net::connection<CustomMsgTypes>> client,
      olc::net::message<CustomMsgTypes>& msg) {
    switch (msg.header.id) {
      case CustomMsgTypes::ServerPing: {
        std::cout << "[" << client->GetID() << "]: Server Ping\n";

        // Simply bounce message back to client
        client->Send(msg);
      } break;

      case CustomMsgTypes::MessageAll: {
        // std::cout << "[" << client->GetID() << "]: Message All\n";

        // Construct a new message and send it to all clients
        olc::net::message<CustomMsgTypes> msg2;
        msg2.header.id = CustomMsgTypes::ServerMessage;

        /*std::cout << "name: " << msg.body[0] << msg.body[1] << msg.body[2]
                  << msg.body[3] << std::endl;

        int temp;
        memcpy(&temp, msg.body.data() + 5, sizeof(int));
        std::cout << "temp: " << temp << std::endl;*/

        if (msg.body[0] != 0) {
          std::cout << "W has been pressed!" << std::endl;
        } else if (msg.body[1] != 0) {
          std::cout << "A has been pressed!" << std::endl;
        } else if (msg.body[2] != 0) {
          std::cout << "S has been pressed!" << std::endl;
        } else if (msg.body[3] != 0) {
          std::cout << "D has been pressed!" << std::endl;
        }

        MessageAllClients(msg2, client);

      } break;
    }
  }

  DWORD tick_ms;
};
