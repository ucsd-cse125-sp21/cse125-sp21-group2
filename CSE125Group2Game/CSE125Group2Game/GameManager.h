#pragma once

#include <GLFW/glfw3.h>

#include "Camera.h"
#include "Model.h"
#include "RenderManager.h"
#include "SceneGraphNode.h"
#include "SceneLoader.h"
#include "Transform.h"
#include "Sound.h"

class GameManager {
 public:
  static const int FORWARD_KEY = GLFW_KEY_W;
  static const int LEFT_KEY = GLFW_KEY_A;
  static const int BACKWARD_KEY = GLFW_KEY_S;
  static const int RIGHT_KEY = GLFW_KEY_D;
  static const int PROJECTILE_KEY = GLFW_KEY_SPACE;

  GameManager(GLFWwindow* window);
  ~GameManager();

  void Update();

  static GameManager* getManager();

  GameObject* unmarshalInfo(char* data);

  void AddPlayer(int clientId);

  void UpdateObject(GameObject* obj);

  bool mKeyPresses[NUM_KEYS];

  void setClientID(int id);

  MeshLoader* mLoader;

  Transform* mPlayerTransform;

  void ResizeCallback(int width, int height);

 private:
  // TODO: fix formatting on this...
  [[deprecated("Replaced by SceneGraph.getByName()")]] SceneGraphNode* findNode(
      GameObject* obj, SceneGraphNode* node);

  std::unique_ptr<RenderManager> mpRenderManager;
  TextureLoader mTLoader;

  Camera* mCamera;
  SceneGraph mScene;
  GLFWwindow* mWindow;
  int mClientId;

  static GameManager* mManager;

  Sound* mSound;
};
