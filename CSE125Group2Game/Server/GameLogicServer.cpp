#include "GameLogicServer.h"

#include <Windows.h>

#include <limits>

#include "Enemy.h"

GameLogicServer* GameLogicServer::mLogicServer;
bool Enemy::hasSpawned = false;

GameLogicServer::GameLogicServer(std::vector<GameObject*> world,
                                 ServerLoader scene, uint16_t tick_ms)
    : mWorld(world), mScene(scene), mTick_ms(tick_ms) {
  for (int i = 0; i < MAX_PLAYERS; i++) {
    bool* keyPresses = (bool*)malloc(NUM_KEYS);

    mKeyPresses.push_back(keyPresses);

    players[i] = NULL;
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
  std::unique_lock<decltype(mMtx)> lk(mMtx);

  if (!Enemy::hasSpawned) {
    std::cout << "creating enemy" << std::endl;

    Enemy* enemy =
        new Enemy(new Transform(glm::vec3(-5, 0, 0), glm::vec3(0, 0, 0),
                                glm::vec3(1, 1, 1)),
                  (char*)"enem0000", 10);

    mWorld.push_back(enemy);
    Enemy::hasSpawned = true;
  }

  // Update player locations
  MovePlayers();

  MoveEnemies();

  SendInfo();

  ResetKeyPresses();
}

void GameLogicServer::MoveEnemies() {
  for (int i = 0; i < mWorld.size(); i++) {
    if (mWorld[i]->getObjectType() == ObjectType::Enemy) {
      // call enemy update
      mWorld[i]->Update();
    }
  }
}

void GameLogicServer::MovePlayers() {
  for (int i = 0; i < MAX_PLAYERS; i++) {
    if (players[i] == NULL) {
      continue;
    }

    int vsp = mKeyPresses[i][GameObject::FORWARD] -
              mKeyPresses[i][GameObject::BACKWARD];

    int hsp =
        mKeyPresses[i][GameObject::RIGHT] - mKeyPresses[i][GameObject::LEFT];

    glm::vec3 velocity = glm::vec3(hsp, vsp, 0);
    if (hsp != 0 || vsp != 0)
      velocity = glm::vec3(0.2) * glm::normalize(velocity);

    std::string clientId = "play000";
    clientId += std::to_string(i);

    // Collision detection
    players[i]->getTransform()->addTranslation(velocity);

    if (DoesCollide(players[i])) {
      // Remove velocity if object collides
      players[i]->getTransform()->addTranslation(-velocity);

      // Step player towards collision boundary
      while (!DoesCollide(players[i])) {
        players[i]->getTransform()->addTranslation(glm::vec3(0.1, 0.1, 0.1) *
                                                   velocity);
      }

      // Move player 0.1 units away from collision
      players[i]->getTransform()->addTranslation(glm::vec3(-0.1, -0.1, -0.1) *
                                                 velocity);

      std::cout << "Collision!" << std::endl;
    } else {
      players[i]->isModified = glm::length(velocity) != 0;
    }
  }

  // if (mKeyPresses[0][0] != 0) {
  //  std::cout << "W has been pressed!" << std::endl;
  //}

  // if (mKeyPresses[0][1] != 0) {
  //  std::cout << "A has been pressed!" << std::endl;
  //}
  // if (mKeyPresses[0][2] != 0) {
  //  std::cout << "S has been pressed! Tick: " << GetTickCount() << std::endl;
  //}
  // if (mKeyPresses[0][3] != 0) {
  //  std::cout << "D has been pressed!" << std::endl;
  //}
}

bool GameLogicServer::DoesCollide(GameObject* obj) {
  // For every gameobject in the world, check if this object collides with
  // anything

  // get 8 points of A in world space
  std::vector<glm::vec3> A = GetCorners(obj);

  // TODO: need to convert everything into player model coordinates then do this
  // method (similar to ray tracing in CSE 167)

  for (int i = 0; i < mWorld.size(); i++) {
    // TODO: only continue if this is the same object (use name)
    if (mWorld[i]->getObjectType() != ObjectType::Default ||
        !strncmp(mWorld[i]->getName(), "root0000", NAME_LEN)) {
      continue;
    }

    std::vector<float> B = GetMinMax(mWorld[i]);

    /*std::cout << "MinX: " << B[MIN_X] << std::endl;
    std::cout << "MinY: " << B[MIN_Y] << std::endl;
    std::cout << "MinZ: " << B[MIN_Z] << std::endl;
    std::cout << "MaxX: " << B[MAX_X] << std::endl;
    std::cout << "MaxY: " << B[MAX_Y] << std::endl;
    std::cout << "MaxZ: " << B[MAX_Z] << std::endl;*/

    // for every point of A, is it in B?
    for (int j = 0; j < 8; j++) {
      /*std::cout << "A point " << j << " x cordinate:" << A[j].x << std::endl;
      std::cout << "A point " << j << " y cordinate:" << A[j].y << std::endl;
      std::cout << "A point " << j << " z cordinate:" << A[j].z << std::endl;*/

      if ((A[j].x >= B[MIN_X] && A[j].x <= B[MAX_X]) &&
          (A[j].y >= B[MIN_Y] && A[j].y <= B[MAX_Y]) &&
          (A[j].z >= B[MIN_Z] && A[j].z <= B[MAX_Z])) {
        // A intersects B
        return true;
      }
    }
  }

  return false;
}

std::vector<float> GameLogicServer::GetMinMax(GameObject* obj) {
  std::vector<glm::vec3> vertices = GetCorners(obj);

  float minX = FLT_MAX, minY = FLT_MAX, minZ = FLT_MAX;
  float maxX = -FLT_MAX, maxY = -FLT_MAX, maxZ = -FLT_MAX;

  for (int j = 0; j < 8; j++) {
    if (vertices[j].x < minX) {
      minX = vertices[j].x;
    }

    if (vertices[j].y < minY) {
      minY = vertices[j].y;
    }

    if (vertices[j].z < minZ) {
      minZ = vertices[j].z;
    }

    if (vertices[j].x > maxX) {
      maxX = vertices[j].x;
    }

    if (vertices[j].y > maxY) {
      maxY = vertices[j].y;
    }

    if (vertices[j].z > maxZ) {
      maxZ = vertices[j].z;
    }
  }

  std::vector<float> result = {minX, minY, minZ, maxX, maxY, maxZ};

  return result;
}

std::vector<glm::vec3> GameLogicServer::GetCorners(GameObject* obj) {
  // TODO: Consider rotation of object when doing bounding box corners
  // TODO: also scale :D
  glm::vec3 center = obj->getTransform()->getTranslation();
  // L,H,W
  glm::vec3 boundingBox = obj->getTransform()->getBBox();

  std::vector<glm::vec3> vertices;
  vertices.push_back(
      center -
      obj->getTransform()->getRotation() *
          (glm::vec3(boundingBox.x, boundingBox.y, boundingBox.z) / 2.0f));

  vertices.push_back(
      center -
      obj->getTransform()->getRotation() *
          (glm::vec3(-boundingBox.x, boundingBox.y, boundingBox.z) / 2.0f));

  vertices.push_back(
      center -
      obj->getTransform()->getRotation() *
          (glm::vec3(boundingBox.x, -boundingBox.y, boundingBox.z) / 2.0f));

  vertices.push_back(
      center -
      obj->getTransform()->getRotation() *
          (glm::vec3(-boundingBox.x, -boundingBox.y, boundingBox.z) / 2.0f));

  vertices.push_back(
      center -
      obj->getTransform()->getRotation() *
          (glm::vec3(boundingBox.x, boundingBox.y, -boundingBox.z) / 2.0f));

  vertices.push_back(
      center -
      obj->getTransform()->getRotation() *
          (glm::vec3(-boundingBox.x, boundingBox.y, -boundingBox.z) / 2.0f));

  vertices.push_back(
      center -
      obj->getTransform()->getRotation() *
          (glm::vec3(boundingBox.x, -boundingBox.y, -boundingBox.z) / 2.0f));

  vertices.push_back(
      center -
      obj->getTransform()->getRotation() *
          (glm::vec3(-boundingBox.x, -boundingBox.y, -boundingBox.z) / 2.0f));

  return vertices;
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
      if (!mWorld[i]->isModified) {
        continue;
      }

      char* data = MarshalInfo(mWorld[i]);  // Marshal data

      // Add message to queue
      // mTestBuffer.push_back(data);

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
