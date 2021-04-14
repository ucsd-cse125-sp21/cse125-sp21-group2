#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <fstream>
#include <iostream>
#include <string>

#include "custom_server.h"
#include "server_helper.h"

using namespace std;
int main() {
  DWORD before, after, diff;
  uint16_t port = DEFAULT_SERVER_PORT;
  uint16_t tick = DEFAULT_TICK;
  std::string filename = CONFIG_FILE;

  if (!server_read_config(port, tick, filename)) {
    cout << "server couldn't read config file, using default port and tick\n";
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