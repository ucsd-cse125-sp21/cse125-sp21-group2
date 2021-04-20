﻿#pragma once

#include <GLFW/glfw3.h>

#include "Camera.h"
#include "Keyboard.h"
#include "Model.h"
#include "RenderManager.h"
#include "SceneGraphNode.h"
#include "SceneLoader.h"
#include "Transform.h"

class GameManager {
 public:
  GameManager();

  void Update();

 private:
  Camera* mCamera;
  SceneGraphNode* mSceneRoot;
  GLFWwindow* mWindow;
};
