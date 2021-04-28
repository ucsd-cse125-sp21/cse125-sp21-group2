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
  GameManager(GLFWwindow* window);
  ~GameManager();

  void Update();

  static GameManager* getManager();

  GameObject* Unmarshal(char* data);

  void UpdateObject(GameObject* obj);

  bool mKeyPresses[NUM_KEYS];

  void ResizeCallback(int width, int height);

 private:
  SceneGraphNode* findNode(GameObject* obj, SceneGraphNode* node);

  std::unique_ptr<RenderManager> mpRenderManager;
  Camera* mCamera;
  SceneGraph mScene;
  GLFWwindow* mWindow;
  MeshLoader* mLoader;
  Transform* mPlayerTransform;

  static GameManager* mManager;
};
