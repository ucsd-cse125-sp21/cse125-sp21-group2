#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <fstream>
#include <iostream>
#include <string>

#include "GameLogicServer.h"
#include "olc_net.h"
#include "server_helper.h"

class CustomServer : public olc::net::server_interface<CustomMsgTypes> {
 public:
  CustomServer(uint16_t nPort)
      : olc::net::server_interface<CustomMsgTypes>(nPort) {}

  static CustomServer* GetCustomServer() {
    // TODO: MAKE SINGLETON AND UPDATE PLAYER COUNT
    uint16_t port = DEFAULT_SERVER_PORT;
    std::string config_field("port");

    std::string str_port = server_read_config_file(config_field, CONFIG_FILE);

    if (str_port.compare(std::string()) == 0) {
      std::cout << "server couldn't read config file, using default port\n";
    } else {
      int temp_int(std::stoi(str_port));
      if (temp_int <= static_cast<int>(UINT16_MAX) && temp_int >= 0) {
        port = static_cast<uint16_t>(temp_int);
      }
    }

    return new CustomServer(port);
  }

 protected:
  virtual bool OnClientConnect(
      std::shared_ptr<olc::net::connection<CustomMsgTypes>> client) {
    olc::net::message<CustomMsgTypes> msg;
    msg.header.id = CustomMsgTypes::ServerAccept;
    client->Send(msg);

    GameLogicServer* logicServer = GameLogicServer::getLogicServer();

    Transform* transform = new Transform(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0),
                                         glm::vec3(1, 1, 1));

    // TODO: make custom names for players :D && UNDO ObjectType::Default
    GameObject* newPlayer =
        new GameObject(transform, (char*)("play0005"), 10, ObjectType::Player);
    logicServer->AddGameObject(newPlayer);

    return true;
  }

  // TODO: remove player from world on disconnect
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

        GameLogicServer* logicServer = GameLogicServer::getLogicServer();

        for (int i = 0; i < msg.body.size(); i++) {
          if (msg.body[i]) {
            logicServer->UpdateKeyPress(i);
          }
        }

        MessageAllClients(msg2, client);

      } break;
    }
  }
};
