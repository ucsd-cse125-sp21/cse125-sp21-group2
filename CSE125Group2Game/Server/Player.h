#pragma once
#include "Moveable.h"
#include "Projectile.h"

#define PLAYER_DAMAGE 2
#define PLAYER_HEAL_RATE_MS 5000
#define PLAYER_HEAL_AMT 1

class Player : public Moveable {
 public:
  static int numPlayers;

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

  void incrementEnemiesKilled();

  int getEnemiesKilled();

  void setEnemiesKilled(int enemiesKilled);

  ~Player();

  bool forceDelete = false;

 private:
  glm::vec3 mRotationSpeed = glm::vec3(-0.0225, -0.10, 0);

  glm::vec3 mVelocity;

  int mPlayerId;

  unsigned long mRespawnTimeMS = 5000;
  unsigned long mTimeToSpawn;
  unsigned long mLastHeal;
  int mEnemiesKilled;
};

const std::string player_prefix = "play";
