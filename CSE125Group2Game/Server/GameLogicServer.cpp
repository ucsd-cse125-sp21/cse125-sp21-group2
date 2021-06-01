#include "GameLogicServer.h"

#include <Windows.h>

#include <limits>

#include "Cloud.h"
#include "Tower.h"
#include "WaveManager.h"

#define SERVER_CONFIG_ERROR \
  "server couldn't read config file using default values\n"
#define SCENE_JSON "../Shared/scene.json"
#define CONFIG_TXT "../config.txt"

GameLogicServer* GameLogicServer::mLogicServer;
int Player::numPlayers = 0;
void sendEndGameInfo(char* data, int size);
void sendStartGame();
void sendWaitingGame(int currPlayers, int minPlayers);

GameLogicServer::GameLogicServer(std::vector<GameObject*> world,
                                 ServerLoader scene, uint16_t tick_ms,
                                 bool friendlyFire)
    : mWorld(world), mScene(scene), mTick_ms(tick_ms) {
  for (int i = 0; i < MAX_PLAYERS; i++) {
    bool* keyPresses = (bool*)malloc(NUM_KEYS);

    for (int j = 0; j < NUM_KEYS; j++) {
      keyPresses[j] = false;
    }

    playerReady[i] = false;

    mKeyPresses.push_back(keyPresses);

    players[i] = NULL;
  }
  mGameStartTick = GetTickCount();
  mPostGameInfoSent = false;
  mFriendlyFire = friendlyFire;
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
    std::string config_ff("friendlyFire");
    bool friendlyFire = false;

    ServerLoader scene(SCENE_JSON);

    std::vector<GameObject*> world = ServerGraphNode::getRoot()->makeWorld(
        glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));

    std::string str_tick = server_read_config2(config_tick, CONFIG_TXT);

    if (str_tick.compare(std::string()) == 0) {
      std::cout << SERVER_CONFIG_ERROR;
    } else {
      int temp_int(std::stoi(str_tick));
      if (temp_int <= static_cast<int>(UINT16_MAX) && temp_int >= 0) {
        tick = static_cast<uint16_t>(temp_int);
      }
    }

    std::string str_ff = server_read_config2(config_ff, CONFIG_TXT);

    if (str_ff.compare(std::string("true")) == 0) {
      std::cout << "ff trueee" << std::endl;
      friendlyFire = true;
    }

    mLogicServer = new GameLogicServer(world, scene, tick, friendlyFire);

    Tower::spawn();
    Cloud::spawn();
  }
  return mLogicServer;
}

void GameLogicServer::update() {
  std::unique_lock<decltype(mMtx)> lk(mMtx);

  float currTime = ((float)GetTickCount()) / 1000;

  mDeltaTime = currTime - mLastTime;

  mLastTime = currTime;

  // Only update game state if game isn't over
  if (!isGameOver()) {
    // printKeyPresses();

    if (playersReady()) {
      WaveManager::getWaveManager()->update();
      updateEnemies();
    }
    mOctree.update();
    updatePickups();  // Update player locations
    updatePlayers();  // Update player locations
    updateTowers();
    updateClouds();
    updateProjectiles();
    if (!startGame) {
      if (playersReady()) {
        sendStartGame();
        startGame = true;
      } else {
        sendWaitingGame(numReadyPlayers, MIN_PLAYERS);
      }
    }
  } else {
    if (!mPostGameInfoSent) {
      sendEndGame();
      mPostGameInfoSent = true;
    }

    for (int i = 0; i < MAX_PLAYERS; i++) {
      if (players[i] == NULL || !mKeyPresses[i][RESTART]) {
        continue;
      }

      restartGame();
      break;
    }
  }

  sendInfo();

  resetKeyPresses();
}

bool GameLogicServer::playersReady() {
  bool ready = true;
  bool noplayers = true;
  numReadyPlayers = 0;
  for (int i = 0; i < MAX_PLAYERS; i++) {
    if (players[i] == nullptr) {
      continue;
    }

    noplayers = false;
    ready = ready && playerReady[i];
    if (playerReady[i]) {
      numReadyPlayers++;
    }
  }
  return ready && !noplayers &&
         /*Player::numPlayers >= MIN_PLAYERS*/ (numReadyPlayers >= MIN_PLAYERS);
}
bool GameLogicServer::isGameOver() {
  // Game is not over until all towers are destroyed
  for (int i = 0; i < mTowers.size(); i++) {
    if (!mTowers[i]->isDead()) {
      return false;
    }
  }

  return true;
}

void GameLogicServer::restartGame() {
  // Delete enemies and projectiles
  for (int i = 0; i < mWorld.size(); i++) {
    mWorld[i]->setHealth(0);
  }

  for (int i = 0; i < MAX_PLAYERS; i++) {
    if (!players[i]) {
      continue;
    }

    players[i]->reset();
  }

  for (int i = 0; i < mTowers.size(); i++) {
    mTowers[i]->setHealth(DEFAULT_HEALTH);
    mWorld.push_back(mTowers[i]);
    mOctree.insert(mTowers[i]);
  }

  for (int i = 0; i < mClouds.size(); i++) {
    mClouds[i]->setHealth(DEFAULT_HEALTH);
    mWorld.push_back(mClouds[i]);
  }

  WaveManager::getWaveManager()->reset();

  mGameStartTick = GetTickCount();
  mPostGameInfoSent = false;
}

void GameLogicServer::updateEnemies() {
  for (int i = 0; i < mWorld.size(); i++) {
    if (mWorld[i]->isEnemy()) {
      // call enemy update
      mWorld[i]->update();
    }
  }
}
void GameLogicServer::updatePickups() {
  for (int i = 0; i < mWorld.size(); i++) {
    if (mWorld[i]->isPickup()) {
      // call enemy update
      mWorld[i]->update();
    }
  }
}

void GameLogicServer::updateTowers() {
  for (int i = 0; i < mTowers.size(); i++) {
    Tower* tower = mTowers[i];

    if (tower->isDead()) {
      continue;
    }

    GameObject* collider = getCollidingObject(tower);

    if (collider != nullptr && collider->isEnemy()) {
      tower->setHealth(tower->getHealth() - TOWER_DAMAGE);
      // Kill the enemy!
      ((Enemy*)collider)->setHealth(0);

      continue;
    }

    // call tower update
    tower->update();
  }
}

void GameLogicServer::updateClouds() {
  int x;
  for (int i = 0; i < mClouds.size(); i++) {
    Cloud* cloud = mClouds[i];

    cloud->update();
  }
}

void GameLogicServer::updateProjectiles() {
  for (int i = 0; i < mWorld.size(); i++) {
    if (mWorld[i]->isProjectile()) {
      Projectile* proj = (Projectile*)mWorld[i];
      GameObject* collider = getCollidingObject(mWorld[i]);
      Player* parent = (Player*)proj->getParent();

      // Currently only collides with enemies
      if (collider != nullptr) {
        if (collider->isEnemy()) {
          proj->setHealth(0);
          ((Enemy*)collider)
              ->setHealth(collider->getHealth() -
                          (ENEMY_PROJ_DAMAGE * parent->mDamageMultiplier));

          if (collider->isDead()) {
            parent->incrementEnemiesKilled();
          }
        } else if (collider->isPlayer()) {
          proj->setHealth(0);
          ((Player*)collider)
              ->setHealth(collider->getHealth() -
                          (PLAYER_PROJ_DAMAGE * parent->mDamageMultiplier));

          if (collider->isDead()) {
            parent->incrementEnemiesKilled();
          }
        }
        continue;
      }

      // call projectile update
      mWorld[i]->update();
    }
  }
}

void GameLogicServer::updatePlayers() {
  for (int i = 0; i < MAX_PLAYERS; i++) {
    if (players[i] == NULL) {
      continue;
    }

    players[i]->update();

    if (players[i]->isDead()) {
      continue;
    }

    if (mKeyPresses[i][READY]) {
      playerReady[i] = true;
    }

    if (mKeyPresses[i][SHOOT]) {
      // std::cout << "Player " << i << " wants to spawn a projectile!!";
      Projectile::spawnProjectile(players[i]);
    }

    handlePlayerCollision(i);
  }
}

void GameLogicServer::handlePlayerCollision(int playerIndex) {
  updatePlayerPosition(playerIndex);

  Player* player = players[playerIndex];

  // Collision detection
  GameObject* collidedObj = getCollidingObject(player);
  if (!collidedObj) {
    return;
  }

  // std::cout << "Collision with: " << collidedObj->getName() << std::endl;

  if (collidedObj->isEnemy()) {
    ((Enemy*)collidedObj)->setHealth(0);
    player->incrementEnemiesKilled();
    player->setHealth(player->getHealth() - PLAYER_DAMAGE);
  } else if (collidedObj->isPickup()) {
    player->addPickup((Pickup*)collidedObj);
  }
}

void GameLogicServer::movePlayerToBoundary(Player* player) {
  player->addTranslation(-player->getVelocity());

  // Step player towards collision boundary
  while (!getCollidingObject(player)) {
    player->addTranslation(glm::vec3(0.1) * player->getVelocity());
  }

  // Move player 0.1 units away from collision
  player->addTranslation(glm::vec3(-0.1) * player->getVelocity());
}

void GameLogicServer::spawnPlayerExplosion(Player* player) {
  float step = 22.5;
  for (float angle = 0; angle < 360; angle += step) {
    Projectile::spawnProjectileAngle(player, angle, 0.20);
  }
}

GameObject* GameLogicServer::getCollidingObject(GameObject* obj) {
  // get 8 points of A in world space

  return mOctree.getCollidingObject(obj);

  // For every gameobject in the world, check if this object collides with
  // anything
  // for (auto iterator = mOctree.begin(obj); iterator != mOctree.end();
  // iterator++) {
  /*for (int i = 0; i < mWorld.size(); i++) {
    // If this object is the root, or has 0 health, or is itself, do not
    // collide
    if (obj->shouldNotCollide(*iterator)) {
      continue;
    }

    if (obj->getTransform()->getBBox().collides(
            (mWorld[i])->getTransform()->getBBox())) {
      return mWorld[i];
    }
    /* if (obj->shouldNotCollide(*iterator)) {
       continue;
     }

     if (obj->getTransform()->getBBox().collides(
             (*iterator)->getTransform()->getBBox())) {
       return *iterator;
     }*/

  return nullptr;
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

void GameLogicServer::addGameObject(GameObject* obj) {
  mWorld.push_back(obj);
  if (!obj->isDefault()) {
    mOctree.insert(obj);
  }
  if (obj->isTower()) {
    mTowers.push_back((Tower*)obj);
  } else if (obj->isCloud()) {
    mClouds.push_back((Cloud*)obj);
  }
}

void GameLogicServer::sendInfo() {
  for (int i = 0; i < mWorld.size(); i++) {
    // Only send info for moving objects
    if (!mWorld[i]->isModifiable() || !mWorld[i]->mIsModified) {
      continue;
    }

    char* data = marshalInfo(mWorld[i]);  // Marshal data
    mTestBuffer.push_back(data);

    // If the enemy has health 0, remove it from the world
    // Allow player to be deleted if they disconnected
    if ((mWorld[i]->isDead() && !mWorld[i]->isPlayer()) ||
        (mWorld[i]->isPlayer() && ((Player*)mWorld[i])->forceDelete)) {
      deleteObject(i);
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

  mOctree.remove(mWorld[worldIndex]);
  mWorld.erase(mWorld.begin() + worldIndex);

  // Don't delete towers
  if (!tmpObj->isTower()) {
    delete tmpObj;
  }
}

char* GameLogicServer::marshalInfo(GameObject* obj) {
  // 1) 8 byte char[] name
  // 32 bit (4 byte) floats
  // 2) Transform: 3 floats for location, 3 for rotation, 3 for scale
  // 3) 4 byte int health 48 bytes

  char* info = (char*)malloc(MESSAGE_SIZE);
  char* tmpInfo = info;

  memcpy(tmpInfo, obj->getName().c_str(), NAME_LEN);  // Copy name into data

  tmpInfo += NAME_LEN;

  glm::mat4 model = obj->getTransform()->getModel();

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      memcpy(tmpInfo, &(model[j][i]), FLOAT_SIZE);
      tmpInfo += FLOAT_SIZE;
    }
  }

  int health = obj->getHealth();
  memcpy(tmpInfo, &(health), INT_SIZE);
  tmpInfo += INT_SIZE;

  glm::vec3 bb = obj->getTransform()->getBBoxLens();
  memcpy(tmpInfo, &(bb.x), FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;
  memcpy(tmpInfo, &(bb.y), FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;
  memcpy(tmpInfo, &(bb.z), FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;

  ObjectType type = obj->getObjectType();
  memcpy(tmpInfo, &(type), TYPE_SIZE);
  tmpInfo += TYPE_SIZE;

  memcpy(tmpInfo, &(obj->mModifier), INT_SIZE);
  tmpInfo += INT_SIZE;

  return info;
}

void GameLogicServer::printKeyPresses() {
  for (int i = 0; i < MAX_PLAYERS; i++) {
    if (players[i] == NULL) {
      continue;
    }

    std::cout << "key presses for player " << i << std::endl;

    std::cout << "forward: " << mKeyPresses[i][FORWARD];
    std::cout << "backward: " << mKeyPresses[i][BACKWARD];
    std::cout << "left: " << mKeyPresses[i][LEFT];
    std::cout << "right: " << mKeyPresses[i][RIGHT];
    std::cout << "shoot: " << mKeyPresses[i][SHOOT];
    std::cout << "restart: " << mKeyPresses[i][RESTART] << std::endl;
  }
}

void GameLogicServer::updatePlayerPosition(int playerId) {
  int vsp = getVerticalInput(playerId);
  int hsp = getHorizontalInput(playerId);

  glm::vec3 velocity(vsp, hsp, 0);

  if (hsp != 0 || vsp != 0) {
    velocity = players[playerId]->getRotationSpeed() * glm::normalize(velocity);
    players[playerId]->move(velocity);
  }
}

int GameLogicServer::getVerticalInput(int playerId) {
  return mKeyPresses[playerId][FORWARD] - mKeyPresses[playerId][BACKWARD];
}
int GameLogicServer::getHorizontalInput(int playerId) {
  return mKeyPresses[playerId][RIGHT] - mKeyPresses[playerId][LEFT];
}

void GameLogicServer::sendEndGame() {
  /*
   * 1) 4 byte DWORD timeEllapse
   * 2) 4 byte int highScore
   * 3) 4 byte int totalEnemyKilled
   * 4) 4 byte int MVP player id
   * 5) 4 byte int numPlayers
   * 6) numPlayers * 4 byte ints enemiesKilledPerPlayer
   * 7) numPlayers * 4 byte ints timesDiedDuringTheGame
   */

  int numPlayers = 0;
  int totalEnemyKilled = 0;
  int mvpIndex = 0;
  int maxEnemiesKilled = 0;

  std::vector<int> enemiesKilledPerPlayer;
  std::vector<int> numRespawnedPerPlayer;

  for (int i = 0; i < MAX_PLAYERS; i++) {
    if (players[i] == NULL) {
      continue;
    }

    numPlayers++;
    totalEnemyKilled += players[i]->mEnemiesKilled;

    enemiesKilledPerPlayer.push_back(players[i]->mEnemiesKilled);
    numRespawnedPerPlayer.push_back(players[i]->mNumRespawned);

    if (players[i]->mEnemiesKilled > maxEnemiesKilled) {
      maxEnemiesKilled = players[i]->mEnemiesKilled;
      mvpIndex = i;
    }
  }

  int message_size = DWORD_SIZE + 4 * INT_SIZE + 2 * numPlayers * INT_SIZE;

  char* info = (char*)malloc(message_size);
  char* tmpInfo = info;

  DWORD timeSurvived = GetTickCount() - mGameStartTick;

  memcpy(tmpInfo, &timeSurvived, DWORD_SIZE);

  tmpInfo += DWORD_SIZE;

  // TODO: determine a better way to calculate score?
  int highScore = WaveManager::getWaveManager()->mWavesCompleted;
  memcpy(tmpInfo, &(highScore), INT_SIZE);
  tmpInfo += INT_SIZE;

  memcpy(tmpInfo, &(totalEnemyKilled), INT_SIZE);
  tmpInfo += INT_SIZE;

  int mvpPlayerID = players[mvpIndex]->getPlayerId();

  memcpy(tmpInfo, &(mvpPlayerID), INT_SIZE);
  tmpInfo += INT_SIZE;

  std::cout << "numPlayers " << numPlayers << std::endl;
  memcpy(tmpInfo, &(numPlayers), INT_SIZE);
  tmpInfo += INT_SIZE;

  for (int i = 0; i < enemiesKilledPerPlayer.size(); i++) {
    memcpy(tmpInfo, &(enemiesKilledPerPlayer[i]), INT_SIZE);
    tmpInfo += INT_SIZE;
  }

  for (int i = 0; i < numRespawnedPerPlayer.size(); i++) {
    memcpy(tmpInfo, &(numRespawnedPerPlayer[i]), INT_SIZE);
    tmpInfo += INT_SIZE;
  }

  sendEndGameInfo(info, message_size);
}
