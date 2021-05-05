﻿#include "GameLogicServer.h"

#include <Windows.h>

#include <limits>

#include "Projectile.h"
#include "WaveManager.h"

GameLogicServer* GameLogicServer::mLogicServer;

GameLogicServer::GameLogicServer(std::vector<GameObject*> world,
                                 ServerLoader scene, uint16_t tick_ms)
    : mWorld(world), mScene(scene), mTick_ms(tick_ms) {
  for (int i = 0; i < MAX_PLAYERS; i++) {
    bool* keyPresses = (bool*)malloc(NUM_KEYS);

    for (int j = 0; j < NUM_KEYS; j++) {
      keyPresses[j] = false;
    }

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

void GameLogicServer::update() {
  std::cerr << "Frame " << rand() << std::endl;
  std::unique_lock<decltype(mMtx)> lk(mMtx);

  // printKeyPresses();
  WaveManager::getWaveManager()->update();

  // 1) Player's, 2) enemies, 3) projectile

  // Update player locations
  updatePlayers();

  updateEnemies();

  // TODO: should we create projectiles before moving players or after? moving
  // changes their forward vector
  updateProjectiles();

  sendInfo();

  resetKeyPresses();
}

void GameLogicServer::updateEnemies() {
  for (int i = 0; i < mWorld.size(); i++) {
    if (mWorld[i]->isEnemy()) {
      // call enemy update
      mWorld[i]->update();
    }
  }
}

void GameLogicServer::updateProjectiles() {
  for (int i = 0; i < MAX_PLAYERS; i++) {
    if (players[i] == NULL) {
      continue;
    }

    if (mKeyPresses[i][GameObject::SHOOT]) {
      // std::cout << "Player " << i << " wants to spawn a projectile!!";
      Projectile::spawnProjectile(players[i]);
    }
  }

  for (int i = 0; i < mWorld.size(); i++) {
    // if (mWorld[i]->getObjectType() == ObjectType::Projectile) {
    if (mWorld[i]->isProjectile()) {
      GameObject* collider = doesCollide(mWorld[i]);

      if (collider != nullptr) {
        mWorld[i]->setHealth(0);
        collider->setHealth(collider->getHealth() - 5);
        continue;
      }

      // call enemy update
      mWorld[i]->update();
    }
  }
}

void GameLogicServer::updatePlayers() {
  for (int i = 0; i < MAX_PLAYERS; i++) {
    if (players[i] == NULL) {
      continue;
    }

    handlePlayerCollision(i);
  }
}

void GameLogicServer::handlePlayerCollision(int playerIndex) {
  int vsp = mKeyPresses[playerIndex][GameObject::FORWARD] -
            mKeyPresses[playerIndex][GameObject::BACKWARD];

  int hsp = mKeyPresses[playerIndex][GameObject::RIGHT] -
            mKeyPresses[playerIndex][GameObject::LEFT];

  glm::vec3 velocity = glm::vec3(hsp, vsp, 0);
  if (hsp != 0 || vsp != 0)
    velocity = glm::vec3(0.4) * glm::normalize(velocity);

  std::string clientId = "play000";
  clientId += std::to_string(playerIndex);

  GameObject* player = players[playerIndex];

  // Collision detection
  player->addTranslation(velocity);

  GameObject* collidedObj = doesCollide(player);
  if (!collidedObj) {
    return;
  }

  std::string name = collidedObj->getName();

  std::cout << "Collision with: " << name << std::endl;

  if (collidedObj->isEnemy()) {
    // Set enemy health to 0
    collidedObj->setHealth(0);
  } else if (collidedObj->isDefault()) {
    // Collision with scene object
    // Remove velocity if object collides
    player->addTranslation(-velocity);

    // Step player towards collision boundary
    while (!doesCollide(player)) {
      player->addTranslation(glm::vec3(0.1, 0.1, 0.1) * velocity);
    }

    // Move player 0.1 units away from collision
    player->addTranslation(glm::vec3(-0.1, -0.1, -0.1) * velocity);
  }
}

GameObject* GameLogicServer::doesCollide(GameObject* obj) {
  // get 8 points of A in world space
  std::vector<glm::vec3> A = getCorners(obj);

  // For every gameobject in the world, check if this object collides with
  // anything
  for (int i = 0; i < mWorld.size(); i++) {
    // If this object is the root, or has 0 health, or is itself, do not collide
    if (obj->shouldNotCollide(mWorld[i])) {
      continue;
    }

    std::vector<float> B = getMinMax(mWorld[i]);

    // For every point of A, is it in B?
    for (int j = 0; j < 8; j++) {
      if ((A[j].x >= B[MIN_X] && A[j].x <= B[MAX_X]) &&
          (A[j].y >= B[MIN_Y] && A[j].y <= B[MAX_Y]) &&
          (A[j].z >= B[MIN_Z] && A[j].z <= B[MAX_Z])) {
        // A intersects B
        return mWorld[i];
      }
    }
  }

  return nullptr;
}

std::vector<float> GameLogicServer::getMinMax(GameObject* obj) {
  std::vector<glm::vec3> vertices = getCorners(obj);

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

std::vector<glm::vec3> GameLogicServer::getCorners(GameObject* obj) {
  // TODO: Consider rotation of object when doing bounding box corners
  // TODO: also scale :D
  glm::vec3 center = obj->getTransform()->getTranslation();
  // L,H,W
  glm::vec3 boundingBox = -1.0f * obj->getTransform()->getBBox();

  std::vector<glm::vec3> vertices;
  vertices.push_back(
      glm::vec3(obj->getTransform()->getModel() *
                glm::vec4(boundingBox.x, boundingBox.y, boundingBox.z, 1.0f)));
  vertices.push_back(
      glm::vec3(obj->getTransform()->getModel() *
                glm::vec4(-boundingBox.x, boundingBox.y, boundingBox.z, 1.0f)));
  vertices.push_back(
      glm::vec3(obj->getTransform()->getModel() *
                glm::vec4(boundingBox.x, -boundingBox.y, boundingBox.z, 1.0f)));
  vertices.push_back(glm::vec3(
      obj->getTransform()->getModel() *
      glm::vec4(-boundingBox.x, -boundingBox.y, boundingBox.z, 1.0f)));
  vertices.push_back(
      glm::vec3(obj->getTransform()->getModel() *
                glm::vec4(boundingBox.x, boundingBox.y, -boundingBox.z, 1.0f)));
  vertices.push_back(glm::vec3(
      obj->getTransform()->getModel() *
      glm::vec4(-boundingBox.x, boundingBox.y, -boundingBox.z, 1.0f)));
  vertices.push_back(glm::vec3(
      obj->getTransform()->getModel() *
      glm::vec4(boundingBox.x, -boundingBox.y, -boundingBox.z, 1.0f)));
  vertices.push_back(glm::vec3(
      obj->getTransform()->getModel() *
      glm::vec4(-boundingBox.x, -boundingBox.y, -boundingBox.z, 1.0f)));

  // smol bbs
  /*
  vertices.push_back(center -
                     glm::vec3(boundingBox.x, boundingBox.y, boundingBox.z));
  vertices.push_back(center -
                     glm::vec3(boundingBox.x, -boundingBox.y, boundingBox.z));
  vertices.push_back(center -
                     glm::vec3(-boundingBox.x, -boundingBox.y, boundingBox.z));
  vertices.push_back(center -
                     glm::vec3(boundingBox.x, boundingBox.y, -boundingBox.z));
  vertices.push_back(center -
                     glm::vec3(-boundingBox.x, boundingBox.y, boundingBox.z));
  vertices.push_back(center -
                     glm::vec3(-boundingBox.x, boundingBox.y, -boundingBox.z));
  vertices.push_back(center -
                     glm::vec3(boundingBox.x, -boundingBox.y, -boundingBox.z));
  vertices.push_back(center -
                     glm::vec3(-boundingBox.x, -boundingBox.y, -boundingBox.z));

   */

  return vertices;
}

void GameLogicServer::printWorld() {
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
uint16_t GameLogicServer::getServerTick() { return mTick_ms; }
std::vector<GameObject*> GameLogicServer::getWorld() { return mWorld; }
ServerLoader GameLogicServer::getScene() { return mScene; }

void GameLogicServer::updateKeyPress(int keyID, int playerID) {
  // Not a priorty update
  std::unique_lock<decltype(mMtx)> lk(mMtx);

  mKeyPresses[playerID][keyID] = true;
  // implicit release of lock when lk is deconstructed
}

// Note: Lock is acquired in Update already
void GameLogicServer::resetKeyPresses() {
  assert(mMtx.isLocked());
  // Not a priorty update
  for (int player = 0; player < MAX_PLAYERS; player++) {
    for (int i = 0; i < NUM_KEYS; i++) {
      mKeyPresses[player][i] = false;
    }
  }
}

void GameLogicServer::addGameObject(GameObject* obj) { mWorld.push_back(obj); }

void GameLogicServer::sendInfo() {
  for (int i = 0; i < mWorld.size(); i++) {
    // Only send info for moving objects
    if (mWorld[i]->isPlayer() || mWorld[i]->isEnemy() ||
        mWorld[i]->isProjectile()) {
      // if (mWorld[i]->isProjectile()) {
      //  std::cout << "sending projectile!!!" << std::endl;
      //  if (!mWorld[i]->mIsModified) {
      //    std::cout << "hmmm projectile NOT modified???" << std::endl;
      //    continue;
      //  }
      //}
      if (!mWorld[i]->mIsModified) {
        continue;
      }

      char* data = marshalInfo(mWorld[i]);  // Marshal data
      data >> mSendingBuffer;               // Add message to queue

      // If the enemy has health 0, remove it from the world
      if (mWorld[i]->getHealth() == 0) {
        deleteObject(i);
      }
    }
  }
}

void GameLogicServer::deleteObject(int worldIndex) {
  GameObject* tmpObj = mWorld[worldIndex];

  // Remove from WaveManager if this is an enemy
  if (tmpObj->isEnemy()) {
    WaveManager::getWaveManager()->removeEnemy((Enemy*)tmpObj);
  }

  mWorld.erase(mWorld.begin() + worldIndex);
  delete tmpObj;
}

char* GameLogicServer::marshalInfo(GameObject* obj) {
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
  tmpInfo += INT_SIZE;

  glm::vec3 bb = obj->getTransform()->getBBox();
  memcpy(tmpInfo, &(bb.x), FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;
  memcpy(tmpInfo, &(bb.y), FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;
  memcpy(tmpInfo, &(bb.z), FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;

  return info;
}

void GameLogicServer::printKeyPresses() {
  for (int i = 0; i < MAX_PLAYERS; i++) {
    if (players[i] == NULL) {
      continue;
    }

    std::cout << "key presses for player " << i << std::endl;

    std::cout << "forward: " << mKeyPresses[i][GameObject::FORWARD];
    std::cout << "backward: " << mKeyPresses[i][GameObject::BACKWARD];
    std::cout << "left: " << mKeyPresses[i][GameObject::LEFT];
    std::cout << "right: " << mKeyPresses[i][GameObject::RIGHT];
    std::cout << "shoot: " << mKeyPresses[i][GameObject::SHOOT];
  }
}
