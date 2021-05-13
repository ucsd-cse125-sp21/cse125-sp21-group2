#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <fstream>
#include <iostream>
#include <string>

#include "GameLogicServer.h"
#include "Player.h"
#include "olc_net.h"
#include "server_helper.h"

using namespace olc::net;
using namespace std;

class NetworkServer : public server_interface<CustomMsgTypes> {
 public:
  NetworkServer(uint16_t nPort) : server_interface<CustomMsgTypes>(nPort) {}

  static NetworkServer* GetNetworkServer();

 protected:
  virtual void AddClient(int id);

  virtual bool OnClientConnect(shared_ptr<connection<CustomMsgTypes>> client);
  virtual void OnClientDisconnect(
      shared_ptr<connection<CustomMsgTypes>> client);

  virtual void OnMessage(shared_ptr<connection<CustomMsgTypes>> client,
                         message<CustomMsgTypes>& msg);

  vector<connection<CustomMsgTypes>> clients;

  // converts from net.getID() to our ID's
  unordered_map<int, int>* serverToLogicID = new unordered_map<int, int>;

  int numPlayers = 0;

 private:
  static NetworkServer* mNetServer;
};

NetworkServer* NetworkServer::GetNetworkServer() {
  // TODO: UPDATE PLAYER COUNT

  if (!mNetServer) {
    uint16_t port = DEFAULT_SERVER_PORT;
    string config_field("port");

    string str_port = server_read_config_file(config_field, CONFIG_FILE);

    if (str_port.compare(string()) == 0) {
      cout << "server couldn't read config file, using default port\n";
    } else {
      int temp_int(stoi(str_port));
      if (temp_int <= static_cast<int>(UINT16_MAX) && temp_int >= 0) {
        port = static_cast<uint16_t>(temp_int);
      }
    }

    mNetServer = new NetworkServer(port);
  }

  return mNetServer;
}

void NetworkServer::AddClient(int id) {
  serverToLogicID->insert(make_pair(id, numPlayers++));

  cout << "Adding ID: " << id << endl;
}

bool NetworkServer::OnClientConnect(
    shared_ptr<connection<CustomMsgTypes>> client) {
  message<CustomMsgTypes> msg;
  msg.header.id = CustomMsgTypes::ServerAccept;
  msg << numPlayers;  // Tell client their player ID
  client->Send(msg);

  GameLogicServer* logicServer = GameLogicServer::getLogicServer();

  Transform* transform = new Transform(glm::vec3(0, RADIUS, 0), glm::vec3(0),
                                       glm::vec3(.5), glm::vec3(0.5 * .5));

  Player* newPlayer =
      new Player(transform, Player::makeName(numPlayers), 10, numPlayers);
  logicServer->addGameObject(newPlayer);

  logicServer->players[numPlayers] = newPlayer;

  return true;
}

// TODO: remove player from world on disconnect
// Called when a client appears to have disconnected
void NetworkServer::OnClientDisconnect(
    shared_ptr<connection<CustomMsgTypes>> client) {
  cout << "Removing client [" << client->GetID() << "]\n";
  GameLogicServer::getLogicServer()
      ->players[serverToLogicID->find(client->GetID())->second]
      ->setHealth(0);
}

// Called when a message arrives
void NetworkServer::OnMessage(shared_ptr<connection<CustomMsgTypes>> client,
                              message<CustomMsgTypes>& msg) {
  switch (msg.header.id) {
    case CustomMsgTypes::ServerPing: {
      cout << "[" << client->GetID() << "]: Server Ping\n";

      // Simply bounce message back to client
      client->Send(msg);
    } break;

    case CustomMsgTypes::ClientMessage: {
      // Construct a new message and send it to all clients
      // cout << "Received message from client" << endl;
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
