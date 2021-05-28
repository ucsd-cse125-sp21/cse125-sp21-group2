#pragma once
#include "Moveable.h"
#include "Projectile.h"

#define PLAYER_DAMAGE 20
#define PLAYER_HEAL_RATE_MS 5000
#define MAX_PLAYER_HEAL_AMT 10
#define MIN_PLAYER_HEAL_AMT 2
#define MID_PLAYER_HEAL_AMT 5
#define PLAYER_HEAL_STEP 2
#define PLAYER_PICKUP_LENGTH 10000

class Pickup;
enum class PickupType;

class Player : public Moveable {
 public:
  static int numPlayers;

  Player(Transform* transform, std::string name, int health, int id);
  ~Player();

  void update();

  int getId();

  void setId(int id);

  void setVelocity(glm::vec3 velocity);
  glm::vec3 getVelocity();

  static std::string makeName(int id);

  glm::vec3 getRotationSpeed();

  static Player* spawnPlayer(int playerId);

  bool shouldNotCollide(GameObject* obj);

  void setHealth(int amt);
  void healPlayer();
  void setRespawn();
  void reset();

  void incrementEnemiesKilled();
  int getEnemiesKilled();
  void setEnemiesKilled(int enemiesKilled);

  void addPickup(Pickup* pickup);
  void removePickup();

  PickupType mPickup;
  unsigned long mPickupEndTime = 0;
  int mDamageMultiplier = 1;
  int mEnemiesKilled;
  float mSpeedMultiplier = 1;

  bool forceDelete = false;
  static const std::string player_prefix;
  bool mShouldHeal;
  int mNumRespawned;
  int mPlayerHealAmt;

 private:
  glm::vec3 mRotationSpeed = SPEED_MULTIPLIER * glm::vec3(-0.0225, -0.10, 0);

  glm::vec3 mVelocity;

  int mPlayerId;

  unsigned long mRespawnTimeMS = 5000;
  unsigned long mTimeToSpawn;
  unsigned long mLastHeal;
};

const std::string player_prefix = "play";
