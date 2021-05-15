#pragma once
#include "Moveable.h"
#include "Projectile.h"

#define PLAYER_DAMAGE 2
#define PLAYER_HEAL_RATE_MS 5000
#define PLAYER_HEAL_AMT 1

class Player : public Moveable {
 public:
  Player(Transform* transform, std::string name, int health, int id);

  void update();

  int getId();

  void setId(int id);

  void setVelocity(glm::vec3 velocity);
  glm::vec3 getVelocity();

  static std::string makeName(int id);

  glm::vec3 getRotationSpeed();

  static const std::string player_prefix;

  static Player* spawnPlayer(int playerId);

  bool shouldNotCollide(GameObject* obj);

  void setHealth(int amt);

 private:
  glm::vec3 mRotationSpeed = glm::vec3(-0.04, -0.12, 0);

  glm::vec3 mVelocity;

  int mPlayerId;

  unsigned long mRespawnTimeMS = 5000;
  unsigned long mTimeToSpawn;
  unsigned long mLastHeal;
};
const std::string player_prefix = "play";
