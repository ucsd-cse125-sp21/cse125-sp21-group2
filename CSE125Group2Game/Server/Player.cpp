#include "Player.h"

Player::Player(Transform* transform, std::string name, int health, int id)
    : Moveable(transform, name, health) {
  mPlayerId = id;
}

void Player::update() {
  // todo
}

int Player::getId() { return mPlayerId; }

void Player::setId(int id) { mPlayerId = id; }
