#include "GameLogicServer.h"

GameLogicServer* GameLogicServer::mLogicServer;

GameLogicServer::GameLogicServer(std::vector<GameObject*> world,
                                 ServerLoader scene, uint16_t tick_ms)
    : mWorld(world), mScene(scene), mTick_ms(tick_ms) {}

std::string server_read_config2(std::string field, std::string filename) {
  std::string line;
  std::ifstream infile;

  infile.open(filename.c_str());
  std::string intermediate;

  // read the config file and fill in port and tick values, if defined
  if (!infile) {
    return std::string();
  }

  while (getline(infile, line)) {
    std::stringstream ss(line);
    std::vector<std::string> tokens;

    while (getline(ss, intermediate, ':')) {
      tokens.push_back(intermediate);
    }

    std::cout << "Token: " << tokens[0] << std::endl;

    if (tokens[0].compare(field) == 0) {
      return tokens[1];
    }
  }

  infile.close();
  return std::string();
}

GameLogicServer* GameLogicServer::getLogicServer() {
  if (!mLogicServer) {
    uint16_t tick = 33;
    std::string config_tick("tick");
    ServerLoader scene("../Shared/scene.json");

    std::vector<GameObject*> world = ServerGraphNode::getRoot()->makeWorld(
        glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));

    std::string str_tick = server_read_config2(config_tick, "../config.txt");

    if (str_tick.compare(std::string()) == 0) {
      std::cout << "server couldn't read config file, using default tick\n";
    } else {
      int temp_int(std::stoi(str_tick));
      if (temp_int <= static_cast<int>(UINT16_MAX) && temp_int >= 0) {
        tick = static_cast<uint16_t>(temp_int);
      }
    }

    mLogicServer = new GameLogicServer(world, scene, tick);
  }
  return mLogicServer;
}

void GameLogicServer::Update() {
  // TODO: Celebrate

  std::unique_lock<decltype(mMtx)> lk(mMtx);

  if (mKeyPresses[0] != 0) {
    std::cout << "W has been pressed!" << std::endl;
  }
  if (mKeyPresses[1] != 0) {
    std::cout << "A has been pressed!" << std::endl;
  }
  if (mKeyPresses[2] != 0) {
    std::cout << "S has been pressed!" << std::endl;
  }
  if (mKeyPresses[3] != 0) {
    std::cout << "D has been pressed!" << std::endl;
  }

  ResetKeyPresses();
}

void GameLogicServer::PrintWorld() {
  for (int i = 0; i < mWorld.size(); i++) {
    std::cout << "World Coordinates: "
              << mWorld[i]->getTransform()->getTranslation().x << ", "
              << mWorld[i]->getTransform()->getTranslation().y << ", "
              << mWorld[i]->getTransform()->getTranslation().z << std::endl;

    std::cout << "World Scale: " << mWorld[i]->getTransform()->getScale().x
              << ", " << mWorld[i]->getTransform()->getScale().y << ", "
              << mWorld[i]->getTransform()->getScale().z << std::endl;
  }
}
uint16_t GameLogicServer::GetServerTick() { return mTick_ms; }
std::vector<GameObject*> GameLogicServer::GetWorld() { return mWorld; }
ServerLoader GameLogicServer::GetScene() { return mScene; }

void GameLogicServer::UpdateKeyPress(int keyID) {
  // Not a priorty update -> see main gameplay loop (TODO)
  std::unique_lock<decltype(mMtx)> lk(mMtx);

  mKeyPresses[keyID] = true;

  // implicit release of lock when lk is deconstructed
}

// Note: Lock is acquired in Update already
void GameLogicServer::ResetKeyPresses() {
  assert(mMtx.isLocked());
  // Not a priorty update -> see main gameplay loop (TODO)
  for (int i = 0; i < NUM_KEYS; i++) {
    mKeyPresses[i] = false;
  }
}
