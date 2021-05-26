//#include "Player.h"

#include "GameLogicServer.h"
#include "Pickup.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

Player::Player(Transform* transform, std::string name, int health, int id)
    : Moveable(transform, name, health, ObjectType::Player) {
  mPlayerId = id;
  Projectile::mTickLastSpawn[name] = 0;
  numPlayers++;
  mPickup = PickupType::None;
  mNumRespawned = 0;
}

Player::~Player() { numPlayers--; }

void Player::update() {
  // If the player is dead,
  if (isDead()) {
    // ... and time to spawn has not been set,
    setRespawn();
    return;
  }

  if (!Pickup::isNone(mPickup) && (mPickupEndTime < GetTickCount())) {
    removePickup();
  }
  // player healing code
  healPlayer();
  // std::cout << "Player healed: " << mHealth << std::endl;
}

void Player::healPlayer() {
  if (mHealth >= DEFAULT_HEALTH ||
      GetTickCount() - mLastHeal < PLAYER_HEAL_RATE_MS) {
    return;
  }

  mLastHeal = GetTickCount();
  mHealth += PLAYER_HEAL_AMT;

  // Ensure health never goes above max
  if (mHealth > DEFAULT_HEALTH) {
    mHealth = DEFAULT_HEALTH;
  }
}

void Player::setRespawn() {
  if (mTimeToSpawn == 0) {
    // set it
    mTimeToSpawn = GetTickCount() + mRespawnTimeMS;
  } else if (mTimeToSpawn < GetTickCount()) {
    // Todo: spawn player in new location potentially or possible ...
    // interesting question!
      mNumRespawned++;
    mTimeToSpawn = 0;
    mHealth = DEFAULT_HEALTH;
  }
}

int Player::getId() { return mPlayerId; }

void Player::setId(int id) { mPlayerId = id; }

glm::vec3 Player::getVelocity() { return mVelocity; }

void Player::setVelocity(glm::vec3 velocity) { mVelocity = velocity; }

std::string Player::makeName(int id) {
  return GameObject::makeName("play", id);
}

glm::vec3 Player::getRotationSpeed() {
  return glm::vec3(mSpeedMultiplier * mRotationSpeed.x, mRotationSpeed.y,
                   mRotationSpeed.z);
}

Player* Player::spawnPlayer(int playerId) {
  Transform* transform =
      new Transform(glm::vec3(0, RADIUS, 0), glm::vec3(0, 0, playerId * -20),
                    glm::vec3(.25), glm::vec3(3));

  Player* newPlayer = new Player(transform, Player::makeName(playerId),
                                 DEFAULT_HEALTH, playerId);

  return newPlayer;
}

bool Player::shouldNotCollide(GameObject* obj) {
  // TODO: Perhaps would be worth making more complex... enemies with
  // projectiles???? interesting question
  return GameObject::shouldNotCollide(obj) ||  // Call super method
         obj->isTower();
}

void Player::setHealth(int amt) {
  // reset timer if taking damage
  if (amt < mHealth) {
    mLastHeal = GetTickCount();
  }

  GameObject::setHealth(amt);
}

void Player::incrementEnemiesKilled() { mEnemiesKilled++; }

int Player::getEnemiesKilled() { return mEnemiesKilled; }

void Player::setEnemiesKilled(int enemiesKilled) {
  mEnemiesKilled = enemiesKilled;
}

void Player::addPickup(Pickup* pickup) {
  if (!Pickup::isNone(mPickup)) {
    removePickup();
  }
  mPickup = pickup->mPickupType;
  // so that pickup gets deleted!!
  pickup->setHealth(0);
  mPickupEndTime = GetTickCount() + PLAYER_PICKUP_LENGTH;

  switch (mPickup) {
    case PickupType::DamageBoost:
      mDamageMultiplier = 3;
      break;

    case PickupType::SpeedBoost:
      mSpeedMultiplier = 2;
      break;

    default:
      break;
  }
}

void Player::reset() {
  removePickup();
  resetModel();
  setHealth(DEFAULT_HEALTH);
  setEnemiesKilled(0);
}

void Player::removePickup() {
  switch (mPickup) {
    case PickupType::DamageBoost:
      mDamageMultiplier = 1;
      break;

    case PickupType::SpeedBoost:
      mSpeedMultiplier = 1;
      break;

    default:
      break;
  }
  mPickup = PickupType::None;
}
