#include "GameLogicServer.h"

#include <Windows.h>

#include <limits>

//#include "Projectile.h"
#include "Tower.h"
#include "WaveManager.h"
#define DEFAULT_TOWER_COUNT 2

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
    int tower_count;
    std::string config_tower("tower_count");
    std::string str_tower = server_read_config2(config_tower, "../config.txt");
    if (str_tower.compare(std::string()) == 0) {
      tower_count = DEFAULT_TOWER_COUNT;
      std::cout << "server couldn't find tower config, use default tower count."
                << std::endl;
    } else {
      tower_count = std::stoi(str_tower);
    }
    Tower::spawn(tower_count);
    std::cout << "Spawning " << tower_count << " towers." << std::endl;
  }
  return mLogicServer;
}

void GameLogicServer::update() {
  std::unique_lock<decltype(mMtx)> lk(mMtx);

  // printKeyPresses();
  WaveManager::getWaveManager()->update();

  // 1) Player's, 2) enemies, 3) projectile

  // Update player locations
  updatePlayers();

  updateEnemies();

  updateTowers();

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

void GameLogicServer::updateTowers() {
  for (int i = 0; i < mWorld.size(); i++) {
    if (mWorld[i]->isTower()) {
      GameObject* collider = getCollidingObject(mWorld[i]);

      if (collider != nullptr &&
          collider->getObjectType() == ObjectType::Enemy) {
        // Take Damage when enemy collide with tower
        // TODO: make damage dynamic
        mWorld[i]->setHealth(mWorld[i]->getHealth() - 0);
        // std::cout << "Tower Health:" << mWorld[i]->getHealth()
        //          << ",collided with " << collider->getName() << std::endl;
        continue;
      }
      // call tower update
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
      GameObject* collider = getCollidingObject(mWorld[i]);

      if (collider != nullptr) {
        mWorld[i]->setHealth(0);
        collider->setHealth(collider->getHealth() - 0.1);
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
  updatePlayerPosition(playerIndex);

  Player* player = players[playerIndex];

  // Collision detection
  // player->/*addTranslation*/ (player->getVelocity());

  GameObject* collidedObj = getCollidingObject(player);
  if (!collidedObj) {
    return;
  }

  std::string name = collidedObj->getName();

  // std::cout << "Collision with: " << name << std::endl;

  if (collidedObj->isEnemy()) {
    collidedObj->setHealth(0);
  } else if (collidedObj->isDefault()) {
    // movePlayerToBoundary(player);
  }
}

void GameLogicServer::movePlayerToBoundary(Player* player) {
  player->addTranslation(-player->getVelocity());

  // Step player towards collision boundary
  while (!getCollidingObject(player)) {
    player->addTranslation(glm::vec3(0.1, 0.1, 0.1) * player->getVelocity());
  }

  // Move player 0.1 units away from collision
  player->addTranslation(glm::vec3(-0.1, -0.1, -0.1) * player->getVelocity());
}

GameObject* GameLogicServer::getCollidingObject(GameObject* obj) {
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
        mWorld[i]->isProjectile() || mWorld[i]->isTower()) {
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
      // data >> mSendingBuffer;               // Add message to queue
      mTestBuffer.push_back(data);

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
  } else if (tmpObj->isPlayer()) {
    players[((Player*)tmpObj)->getId()] = NULL;
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
  if (!info) {
    std::cerr << "bruh" << std::endl;
  }

  char* tmpInfo = info;

  memcpy(tmpInfo, obj->getName().c_str(), NAME_LEN);  // Copy name into data

  tmpInfo += NAME_LEN;

  glm::vec3 pos = obj->getTransform()->getTranslation();
  memcpy(tmpInfo, &(pos.x), FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;
  memcpy(tmpInfo, &(pos.y), FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;
  memcpy(tmpInfo, &(pos.z), FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;

  // TODO: In unmarshling, euler angles -> quat
  // glm::vec3 rot = glm::eulerAngles(obj->getTransform()->getRotation());
  // std::cout << "in server " << rot.x << " " << rot.y << " " << rot.z
  //          << std::endl;

  glm::quat rot = obj->getTransform()->getRotation();

  memcpy(tmpInfo, &(rot.x), FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;
  memcpy(tmpInfo, &(rot.y), FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;
  memcpy(tmpInfo, &(rot.z), FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;
  memcpy(tmpInfo, &(rot.w), FLOAT_SIZE);
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

  ObjectType type = obj->getObjectType();
  memcpy(tmpInfo, &(type), TYPE_SIZE);
  tmpInfo += TYPE_SIZE;

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

void GameLogicServer::updatePlayerPosition(int playerId) {
  int vsp = getVerticalInput(playerId);
  int hsp = getHorizontalInput(playerId);

  glm::vec3 velocity(vsp, hsp, 0);

  if (hsp != 0 || vsp != 0) {
    std::cout << vsp << std::endl;
    velocity = players[playerId]->getRotationSpeed() * glm::normalize(velocity);
    players[playerId]->move(velocity);
  }

  // players[playerId]->setVelocity(velocity);
}

int GameLogicServer::getVerticalInput(int playerId) {
  return mKeyPresses[playerId][GameObject::FORWARD] -
         mKeyPresses[playerId][GameObject::BACKWARD];
}
int GameLogicServer::getHorizontalInput(int playerId) {
  return mKeyPresses[playerId][GameObject::RIGHT] -
         mKeyPresses[playerId][GameObject::LEFT];
}
