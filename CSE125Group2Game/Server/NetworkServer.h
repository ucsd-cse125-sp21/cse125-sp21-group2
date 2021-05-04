#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <fstream>
#include <iostream>
#include <string>

#include "GameLogicServer.h"
#include "olc_net.h"
#include "server_helper.h"

class NetworkServer : public olc::net::server_interface<CustomMsgTypes> {
 public:
  NetworkServer(uint16_t nPort)
      : olc::net::server_interface<CustomMsgTypes>(nPort) {}

  static NetworkServer* GetNetworkServer() {
    // TODO: UPDATE PLAYER COUNT

    if (!mNetServer) {
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

      mNetServer = new NetworkServer(port);
    }

    return mNetServer;
  }

 protected:
  std::vector<olc::net::connection<CustomMsgTypes>> clients;
  // converts from net.getID() to our ID's
  std::unordered_map<int, int>* serverToLogicID =
      new std::unordered_map<int, int>;

  int numPlayers = 0;

  virtual void AddClient(int id) {
    serverToLogicID->insert(std::make_pair(id, numPlayers++));

    std::cout << "Adding ID: " << id << std::endl;
  }

  virtual bool OnClientConnect(
      std::shared_ptr<olc::net::connection<CustomMsgTypes>> client) {
    olc::net::message<CustomMsgTypes> msg;
    msg.header.id = CustomMsgTypes::ServerAccept;
    msg << numPlayers;  // Tell client their player ID
    client->Send(msg);

    GameLogicServer* logicServer = GameLogicServer::getLogicServer();

    Transform* transform =
        new Transform(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0),
                      glm::vec3(1, 1, 1), glm::vec3(0.5, 0.5, 0.5));

    // Assign incoming player a unique ID
    std::string clientId = "play000";
    clientId += std::to_string(numPlayers);

    GameObject* newPlayer =
        new GameObject(transform, (char*)clientId.c_str(), 10,
                       ObjectType::Player, glm::vec3(1, 0, 0));
    logicServer->addGameObject(newPlayer);

    logicServer->players[numPlayers] = newPlayer;

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

      case CustomMsgTypes::ClientMessage: {
        // Construct a new message and send it to all clients
        // std::cout << "Received message from client" << std::endl;
        olc::net::message<CustomMsgTypes> msg2;
        msg2.header.id = CustomMsgTypes::ServerMessage;

        GameLogicServer* logicServer = GameLogicServer::getLogicServer();

        for (int i = 0; i < msg.body.size(); i++) {
          if (msg.body[i]) {
            logicServer->updateKeyPress(
                i, serverToLogicID->find(client->GetID())->second);
          }
        }

      } break;
    }
  }

 private:
  static NetworkServer* mNetServer;
};
