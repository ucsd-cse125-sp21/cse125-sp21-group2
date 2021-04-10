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
        std::cout << "[" << client->GetID() << "]: Message All\n";

        // Construct a new message and send it to all clients
        olc::net::message<CustomMsgTypes> msg;
        msg.header.id = CustomMsgTypes::ServerMessage;
        msg << client->GetID();
        MessageAllClients(msg, client);

      } break;
    }
  }

  DWORD tick_ms;
};

int main() {
  DWORD before, after, diff;
  uint16_t port = 60000;
  uint16_t tick = 10000;
  std::string filename = "server_config.txt";

  std::string line;
  std::ifstream infile;

  infile.open(filename.c_str());
  std::vector<std::string> tokens;
  std::string intermediate;

  // read the config file and fill in port and tick values, if defined
  if (infile) {
    while (getline(infile, line)) {
      std::stringstream ss(line);

      while (getline(ss, intermediate, ':')) {
        tokens.push_back(intermediate);
      }

      if (tokens[0].compare("port") == 0) {
        int temp_int(std::stoi(tokens[1]));

        if (temp_int <= static_cast<int>(UINT16_MAX) && temp_int >= 0) {
          port = static_cast<uint16_t>(temp_int);
        }
      } else if (tokens[0].compare("tick") == 0) {
        int temp_int(std::stoi(tokens[1]));

        if (temp_int <= static_cast<int>(UINT16_MAX) && temp_int >= 0) {
          tick = static_cast<uint16_t>(temp_int);
        }
      }
    }
  } else {
    std::cout << "couldn't open file\n";
    TCHAR Buffer[MAX_PATH];
    DWORD dwRet;
    dwRet = GetCurrentDirectory(MAX_PATH, Buffer);
    std::wcout << Buffer << std::endl;
  }

  std::cout << "port:" << port << "\n";
  std::cout << "tick:" << tick << "\n";

  CustomServer server(port,
                      tick);  // currently the server polls once per second
  server.Start();

  while (1) {
    before = GetTickCount();
    server.Update(-1, true);
    after = GetTickCount();

    diff = after - before;
    std::cout << diff;
    if (server.GetServerTick() >
        diff) {  // need to ensure that server tick is big enough
      Sleep(server.GetServerTick() - diff);
    }
  }

  return 0;
}