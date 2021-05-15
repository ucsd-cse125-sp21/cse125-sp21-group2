//#include "Player.h"

#include "GameLogicServer.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

Player::Player(Transform* transform, std::string name, int health, int id)
    : Moveable(transform, name, health, ObjectType::Player) {
  mPlayerId = id;
  Projectile::mTickLastSpawn[name] = 0;
}

void Player::update() {
  // If the player is dead,
  if (mHealth <= 0) {
    // ... and time to spawn has not been set,
    if (mTimeToSpawn == 0) {
      // set it
      mTimeToSpawn = GetTickCount() + mRespawnTimeMS;
    } else if (mTimeToSpawn < GetTickCount()) {
      // Todo: spawn player in new location potentially or possible ...
      // interesting question!
      mTimeToSpawn = 0;
      mHealth = DEFAULT_HEALTH;
    }
  }
}

int Player::getId() { return mPlayerId; }

void Player::setId(int id) { mPlayerId = id; }

glm::vec3 Player::getVelocity() { return mVelocity; }

void Player::setVelocity(glm::vec3 velocity) { mVelocity = velocity; }

std::string Player::makeName(int id) {
  return GameObject::makeName("play", id);
}

glm::vec3 Player::getRotationSpeed() { return mRotationSpeed; }

Player* Player::spawnPlayer(int playerId) {
  Transform* transform = new Transform(glm::vec3(0, RADIUS, 0), glm::vec3(0),
                                       glm::vec3(.5), glm::vec3(3.5));

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
