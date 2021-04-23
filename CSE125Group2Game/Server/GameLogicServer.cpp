#include "GameLogicServer.h"

GameLogicServer* GameLogicServer::mLogicServer;

GameLogicServer::GameLogicServer(std::vector<GameObject*> world,
                                 ServerLoader scene, uint16_t tick_ms)
    : mWorld(world), mScene(scene), mTick_ms(tick_ms) {
  for (int i = 0; i < MAX_PLAYERS; i++) {
    bool* keyPresses = (bool*)malloc(NUM_KEYS);

    mKeyPresses.push_back(keyPresses);
  }
}

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
  // TOOD: free ur damn memory
  // TODO: create Enemy and Projectile Classes (child of GameObject)
  // (server-side only)

  std::unique_lock<decltype(mMtx)> lk(mMtx);

  Transform* playerTransform = NULL;

  for (int i = 0; i < mWorld.size(); i++) {
    // Find player1 gameobject
    if (mWorld[i]->getName() == "play0000") {
      playerTransform = mWorld[i]->getTransform();
      break;
    }
  }

  int vsp =
      mKeyPresses[0][GameObject::KEY_W] - mKeyPresses[0][GameObject::KEY_S];

  int hsp =
      mKeyPresses[0][GameObject::KEY_D] - mKeyPresses[0][GameObject::KEY_A];

  glm::vec3 velocity = glm::vec3(hsp, vsp, 0);
  if (hsp != 0 || vsp != 0)
    velocity = glm::vec3(0.5) * glm::normalize(velocity);

  for (int i = 0; i < mWorld.size(); i++) {
    if (mWorld[i]->getObjectType() == ObjectType::Player) {
      mWorld[i]->getTransform()->addTranslation(velocity);
      break;
    }
  }
  // playerTransform->addTranslation(velocity);

  /*if (mKeyPresses[0] != 0) {
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
  }*/

  SendInfo();

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

void GameLogicServer::UpdateKeyPress(int keyID, int playerID) {
  // Not a priorty update
  std::unique_lock<decltype(mMtx)> lk(mMtx);

  mKeyPresses[playerID][keyID] = true;
  // implicit release of lock when lk is deconstructed
}

// Note: Lock is acquired in Update already
void GameLogicServer::ResetKeyPresses() {
  assert(mMtx.isLocked());
  // Not a priorty update
  for (int player = 0; player < MAX_PLAYERS; player++) {
    for (int i = 0; i < NUM_KEYS; i++) {
      mKeyPresses[player][i] = false;
    }
  }
}

void GameLogicServer::AddGameObject(GameObject* obj) { mWorld.push_back(obj); }

void GameLogicServer::SendInfo() {
  for (int i = 0; i < mWorld.size(); i++) {
    // Only send info for moving objects
    if (mWorld[i]->getObjectType() == ObjectType::Player ||
        mWorld[i]->getObjectType() == ObjectType::Enemy ||
        mWorld[i]->getObjectType() == ObjectType::Projectile) {
      char* data = MarshalInfo(mWorld[i]);  // Marshal data

      // Add message to queue
      data >> mSendingBuffer;
    }
  }
}

char* GameLogicServer::MarshalInfo(GameObject* obj) {
  // 1) 8 byte char[] name
  // 32 bit (4 byte) floats
  // 2) Transform: 3 floats for location, 3 for rotation, 3 for scale
  // 3) 4 byte int health 48 bytes

  char* info = (char*)malloc(MESSAGE_SIZE);

  char* tmpInfo = info;

  memcpy(tmpInfo, obj->getName(), NAME_LEN);  // Copy name into data

  tmpInfo += NAME_LEN;

  glm::vec3 pos = obj->getTransform()->getTranslation();
  memcpy(tmpInfo, &(pos.x), FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;
  memcpy(tmpInfo, &(pos.y), FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;
  memcpy(tmpInfo, &(pos.z), FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;

  // TODO: In unmarshling, euler angles -> quat
  glm::vec3 rot = glm::eulerAngles(obj->getTransform()->getRotation());
  memcpy(tmpInfo, &(rot.x), FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;
  memcpy(tmpInfo, &(rot.y), FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;
  memcpy(tmpInfo, &(rot.z), FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;

  glm::vec3 scale = obj->getTransform()->getScale();
  memcpy(tmpInfo, &(scale.x), FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;
  memcpy(tmpInfo, &(scale.y), FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;
  memcpy(tmpInfo, &(scale.z), FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;

  int health = obj->getHealth();
  memcpy(tmpInfo, &(health), INT_SIZE);

  return info;
}
