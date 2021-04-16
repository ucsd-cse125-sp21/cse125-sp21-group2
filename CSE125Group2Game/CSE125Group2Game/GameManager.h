#pragma once

#include <GLFW/glfw3.h>

#include "Camera.h"
#include "Mesh.h"
#include "RenderManager.h"
#include "SceneGraphNode.h"
#include "SceneLoader.h"
#include "Transform.h"

#define NUM_KEYS 4

/* TODO:
 *		1) Check reset keys
 *		1.5) Can I create window in getManager????
 *		2) Run at fixed frame rate?
 *		3) Make subfunctions for setting up network stuff?
 */

class GameManager {
 public:
  static const int KEY_W = 0;
  static const int KEY_A = 1;
  static const int KEY_S = 2;
  static const int KEY_D = 3;

  GameManager(GLFWwindow* window);

  void Update();

  static GameManager* getManager();

 private:
  Camera* mCamera;
  SceneGraphNode* mSceneRoot;
  GLFWwindow* mWindow;

  static GameManager* mManager;
};
