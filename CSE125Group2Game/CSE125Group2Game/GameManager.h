#pragma once

#include <GLFW/glfw3.h>

#include "Camera.h"
#include "Model.h"
#include "RenderManager.h"
#include "SceneGraphNode.h"
#include "SceneLoader.h"
#include "Transform.h"

class GameManager {
 public:
  static const int FORWARD_KEY = GLFW_KEY_W;
  static const int LEFT_KEY = GLFW_KEY_A;
  static const int BACKWARD_KEY = GLFW_KEY_S;
  static const int RIGHT_KEY = GLFW_KEY_D;
  static const int PROJECTILE_KEY = GLFW_KEY_B;

  GameManager(GLFWwindow* window);

  void Update();

  static GameManager* getManager();

  GameObject* Unmarshal(char* data);

  void AddPlayer(int clientId);

  void UpdateObject(GameObject* obj);

  bool mKeyPresses[NUM_KEYS];

  void setClientID(int id);

  MeshLoader* mLoader;

  Transform* mPlayerTransform;

 private:
  SceneGraphNode* findNode(GameObject* obj, SceneGraphNode* node);

  Camera* mCamera;
  SceneGraphNode* mSceneRoot;
  GLFWwindow* mWindow;
  int mClientId;

  static GameManager* mManager;
};
