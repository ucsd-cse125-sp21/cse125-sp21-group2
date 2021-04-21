#include <olc_net.h>

#include <iostream>

#include "GameObject.h"

/* TODOs:
 *   1. Probably game shouldn't start will connected, or there should be some
 * thing in the window that says its conencting
 *     - doesn't need to be super polished
 *   2. Refactor key press code to use GLFW callback
 *     - see https://www.glfw.org/docs/latest/input_guide.html#input_keyboard
 *   3. Refactor some code in the client so we don't need to hard-code host and
 * port (config file yeet)
 *   4. Make the server and client actually do something with the data
 */

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
            // Server has responded to a ping request
            /*uint32_t clientID;
            msg >> clientID;
            std::cout << "Hello from [" << clientID << "]\n";*/

            if (msg.body.size() != MESSAGE_SIZE) {
              std::cerr << "Error: Message body size of " << msg.body.size()
                        << " is incorrect in net_client update. Expected "
                        << MESSAGE_SIZE << std::endl;
              // return false;
            }

            char* data = (char*)malloc(MESSAGE_SIZE);

            for (int i = 0; i < MESSAGE_SIZE; i++) {
              data[i] = msg.body[i];
            }

            GameObject* obj = GameManager::getManager()->Unmarshal(data);
            GameManager::getManager()->UpdateObject(obj,
                                                    SceneGraphNode::getRoot());

            char name[NAME_LEN + 1];
            memcpy(name, obj->getName(), NAME_LEN);
            name[NAME_LEN] = '\0';

            std::cout << "Name: " << name << std::endl;
            std::cout << "Health: " << obj->getHealth() << std::endl;

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
