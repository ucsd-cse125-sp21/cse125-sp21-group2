#pragma once

#include <GLFW/glfw3.h>
#include <stdio.h>

#include "Camera.h"
#include "Font.h"
#include "GameEndInfo.h"
#include "Model.h"
#include "ModelLoader.h"
#include "RenderManager.h"
#include "SceneGraphNode.h"
#include "SceneLoader.h"
#include "Sound.h"
#include "Transform.h"

#define PLAYER_MODEL_RED "Assets/models/usership/red/usership-red.obj"
#define PLAYER_MODEL_GREEN "Assets/models/usership/green/usership-green.obj"
#define PLAYER_MODEL_BLUE "Assets/models/usership/blue/usership-blue.obj"
#define PLAYER_MODEL_YELLOW "Assets/models/usership/yellow/usership-yellow.obj"
#define ENEMY_MODEL "Assets/models/usership/geisel/geisel.obj"
#define BEARL_MODEL "Assets/models/enemy/mainEnemyShip/enemyShip.obj"
#define STONEHENGE_MODEL "Assets/models/towers/stonehenge/stonehenge.obj"
#define FALLEN_STAR_MODEL "Assets/models/towers/fallenstar/fallenstar.obj"
#define CLOUD_MODEL "Assets/models/cloud/cloud.obj"
#define PROJECTILE_MODEL "Assets/models/bullet/bullet.obj"
#define DAMAGE_BOOST_MODEL \
  "Assets/models/powerups/positive/attack/positive-attack.obj"
// TODO: UPDATE SPEED BOOST MODEL PATH
#define SPEED_BOOST_MODEL \
  "Assets/models/powerups/positive/speed/positive-speed.obj"
#define INVINCIBILITY_MODEL \
  "Assets/models/powerups/positive/invincibility/positive-invincibility.obj"
#define EXPLOSION_MODEL \
  "Assets/models/powerups/positive/explosion/positive-explosion.obj"
#define DAMAGE_REDUCTION_MODEL \
  "Assets/models/powerups/negative/attack/negative-attack.obj"
#define SPEED_REDUCTION_MODEL \
  "Assets/models/powerups/negative/speed/negative-speed.obj"
#define NO_SHOOTING_MODEL \
  "Assets/models/powerups/negative/explosion/negative-explosion.obj"
#define WEAKNESS_MODEL \
  "Assets/models/powerups/negative/invincibility/negative-invincibility.obj"

#define FORWARD_KEY GLFW_KEY_W
#define LEFT_KEY GLFW_KEY_A
#define BACKWARD_KEY GLFW_KEY_S
#define RIGHT_KEY GLFW_KEY_D
#define PROJECTILE_KEY GLFW_KEY_SPACE
#define RESTART_KEY GLFW_KEY_R
#define READY_KEY GLFW_KEY_ENTER
#define EASTER_KEY GLFW_KEY_LEFT_BRACKET

class GameManager {
 public:
  GameManager(GLFWwindow* window);
  ~GameManager();

  void Update();

  void renderUI();

  void renderGameOverUI();

  void updateKeyPresses(bool keysPressed[5]);

  static GameManager* getManager();

  GameObject* unmarshalInfo(char* data);

  void UpdateObject(GameObject* obj);

  void updateSound(GameObject* foundObject);

  void spawnObject(GameObject* obj, GameObject*& foundObject,
                   SceneGraphNode*& foundNode);

  void addPlayer(GameObject*& foundObject, Model* model);

  void setClientID(int id);

  bool mKeyPresses[NUM_KEYS];

  MeshLoader* mLoader;

  GameObject* mPlayer;

  void ResizeCallback(int width, int height);

  int mClientId;
  bool mGameOver = false;

  int mWaveTimer = 0;
  int mWavesCompleted = 0;
  int mNumEnemies = 0;

  static std::string playerModels[];
  static std::string pickupModels[];
  GameEndInfo* mEndInfo;
  bool mStartGame = false;
  bool mReady = false;

  bool mClientConnected = false;

 private:
  // TODO: fix formatting on this...
  [[deprecated("Replaced by SceneGraph.getByName()")]] SceneGraphNode* findNode(
      GameObject* obj, SceneGraphNode* node);

  std::unique_ptr<RenderManager> mpRenderManager;
  TextureLoader mTLoader;
  ModelLoader mMLoader;

  Camera* mCamera;
  SceneGraph mScene;
  GLFWwindow* mWindow;
  Font* mpFont;

  static GameManager* mManager;

  Sound* mSound;

  bool mEasterMode;
};
