#pragma once

#include <GLFW/glfw3.h>

#include "Camera.h"
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

#define FORWARD_KEY GLFW_KEY_W
#define LEFT_KEY GLFW_KEY_A
#define BACKWARD_KEY GLFW_KEY_S
#define RIGHT_KEY GLFW_KEY_D
#define PROJECTILE_KEY GLFW_KEY_SPACE
#define RESTART_KEY GLFW_KEY_R

class GameManager {
 public:
  GameManager(GLFWwindow* window);
  ~GameManager();

  void Update();

  void renderUI();

  void updateKeyPresses(bool keysPressed[5]);

  static GameManager* getManager();

  GameObject* unmarshalInfo(char* data);

  void UpdateObject(GameObject* obj);

  void setClientID(int id);

  bool mKeyPresses[NUM_KEYS];

  MeshLoader* mLoader;

  GameObject* mPlayer;

  void ResizeCallback(int width, int height);

  int mClientId;

  int mWaveTimer = 0;
  int mWavesCompleted = 0;

  static std::string playerModels[];

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
  UI* mpUI;

  static GameManager* mManager;

  Sound* mSound;
};
