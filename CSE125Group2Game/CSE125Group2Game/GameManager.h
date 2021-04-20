#pragma once

#include <GLFW/glfw3.h>

#include "Camera.h"
#include "Model.h"
#include "RenderManager.h"
#include "SceneGraphNode.h"
#include "SceneLoader.h"
#include "Transform.h"

/* TODO:
 *		1) Check reset keys
 *		1.5) Can I create window in getManager????
 *		2) Run at fixed frame rate?
 *		3) Make subfunctions for setting up network stuff?
 */

class GameManager {
 public:
  GameManager(GLFWwindow* window);

  void Update();

  static GameManager* getManager();
  bool mKeyPresses[NUM_KEYS];

 private:
  Camera* mCamera;
  SceneGraphNode* mSceneRoot;
  GLFWwindow* mWindow;
  MeshLoader* mLoader;

  static GameManager* mManager;
};
