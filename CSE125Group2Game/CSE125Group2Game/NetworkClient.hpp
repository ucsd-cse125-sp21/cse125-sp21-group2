#include <Windows.h>
#include <olc_net.h>

#include <iostream>
#include <queue>
#include <set>
#include <stack>

#include "GameObject.h"

class CustomClient : public olc::net::client_interface<CustomMsgTypes> {
 public:
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
      std::set<std::string> processedObjects;
      std::stack<olc::net::message<CustomMsgTypes>> messageQueue;
      std::stack<olc::net::message<CustomMsgTypes>> acceptMessage;

      // Reads messages in reverse order, allows us to only proccess a specfic
      // object once per tick
      while (!this->Incoming().empty()) {
        auto msg = this->Incoming().pop_front().msg;

        if (msg.header.id == CustomMsgTypes::ServerAccept) {
          acceptMessage.push(msg);
        } else {
          messageQueue.push(msg);
        }
      }

      if (!acceptMessage.empty()) {
        messageQueue.push(acceptMessage.top());
        acceptMessage.pop();
      }

      while (!messageQueue.empty()) {
        // while (!this->Incoming().empty()) {
        //  auto msg = this->Incoming().pop_front().msg;
        auto msg = messageQueue.top();
        messageQueue.pop();

        switch (msg.header.id) {
          case CustomMsgTypes::ServerAccept: {
            // Server has responded to a ping request
            std::cout << "Server Accepted Connection\n";

            // Get client id from message
            int clientId;
            msg >> clientId;

            std::cout << "id recieved: " << clientId << std::endl;

            GameManager::getManager()->mClientConnected = true;

            GameManager::getManager()->mClientId = clientId;

          } break;

          case CustomMsgTypes::WaveTimer: {
            GameManager::getManager()->mGameOver =
                false;  // TODO: FIX THIS HACK

            int currentWaveTimer, wavesCompleted, numEnemies;
            msg >> numEnemies;
            msg >> wavesCompleted;
            msg >> currentWaveTimer;

            GameManager::getManager()->mWaveTimer = currentWaveTimer;
            GameManager::getManager()->mWavesCompleted = wavesCompleted;
            GameManager::getManager()->mNumEnemies = numEnemies;

          } break;

          case CustomMsgTypes::StartGame: {
            // TODO: Lower UI to start game
            GameManager::getManager()->mStartGame = true;

          } break;

          case CustomMsgTypes::EndGame: {
            GameManager::getManager()->mGameOver = true;
            GameEndInfo* endInfo = GameManager::getManager()->mEndInfo;

            char* data = (char*)malloc(msg.body.size());
            char* dataToBeFreed = data;

            for (int i = 0; i < msg.body.size(); i++) {
              data[i] = msg.body[i];
            }

            // time ellapsed in ms
            // DWORD timeEllapsed;
            memcpy(&(endInfo->timeEllapsed), data, DWORD_SIZE);
            data += DWORD_SIZE;

            // int highScore;
            memcpy(&(endInfo->highScore), data, INT_SIZE);
            data += INT_SIZE;

            // int totalEnemiesKilled;
            memcpy(&(endInfo->totalEnemiesKilled), data, INT_SIZE);
            data += INT_SIZE;

            // int mvpPlayerID;
            memcpy(&(endInfo->mvpPlayerID), data, INT_SIZE);
            data += INT_SIZE;

            // int numPlayers;
            memcpy(&(endInfo->numPlayers), data, INT_SIZE);
            data += INT_SIZE;

            // std::vector<int> enemiesKilledPerPlayer;

            for (int i = 0; i < endInfo->numPlayers; i++) {
              int enemiesKilled;
              memcpy(&enemiesKilled, data, INT_SIZE);
              data += INT_SIZE;
              endInfo->mEnemiesKilledPerPlayer.push_back(enemiesKilled);
            }

            // std::vector<int> numRespawnedPerPlayer;  // AKA NumDied

            for (int i = 0; i < endInfo->numPlayers; i++) {
              int numRespawned;
              memcpy(&numRespawned, data, INT_SIZE);
              data += INT_SIZE;
              endInfo->mNumRespawnedPerPlayer.push_back(numRespawned);
            }

            std::cout << "timeEllapsed " << endInfo->timeEllapsed << std::endl;
            std::cout << "highScore " << endInfo->highScore << std::endl;
            std::cout << "totalEnemiesKilled " << endInfo->totalEnemiesKilled
                      << std::endl;
            std::cout << "mvpPlayerID " << endInfo->mvpPlayerID << std::endl;
            std::cout << "numPlayers " << endInfo->numPlayers << std::endl;

            // TODO: Do something with the data

            free(dataToBeFreed);
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

            // Only update object if it hasn't been processed this tick yet
            if (!processedObjects.count(obj->getName())) {
              processedObjects.insert(obj->getName());

              GameManager::getManager()->UpdateObject(obj);
            } else {
              // std::cout << "Actually got here :D" << std::endl;
            }

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
