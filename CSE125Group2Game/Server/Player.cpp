//#include "Player.h"

#include "GameLogicServer.h"

Player::Player(Transform* transform, std::string name, int health, int id)
    : Moveable(transform, name, health, ObjectType::Player) {
  mPlayerId = id;
  Projectile::mTickLastSpawn[name] = 0;
}

void Player::update() {}

int Player::getId() { return mPlayerId; }

void Player::setId(int id) { mPlayerId = id; }
int Player::getVsp() { return mVelocity.y; }
int Player::getHsp() { return mVelocity.x; }
glm::vec3 Player::getVelocity() { return mVelocity; }

void Player::setVelocity(glm::vec3 velocity) { mVelocity = velocity; }

std::string Player::makeName(int id) {
  return GameObject::makeName("play", id);
}

glm::vec3 Player::getRotationSpeed() { return mRotationSpeed; }
