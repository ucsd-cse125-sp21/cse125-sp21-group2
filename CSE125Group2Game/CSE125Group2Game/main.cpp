// main.cpp : This file contains the 'main' function. Program execution begins
// and ends there.
//

#include <GLFW/glfw3.h>

#include <iostream>

#include "Camera.h"
#include "Model.h"
#include "RenderManager.h"
#include "SceneGraphNode.h"
#include "SceneLoader.h"
#include "Transform.h"
#include "net_client.hpp"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  RenderManager::get().setViewportSize(width, height);
}

/*
 * 1. Set up ticks, lets do 30 per second (frame rate still unbounded)
 * 2. Refactor some of the glfw window code into a window class
 * 3. Refactor singletons into a Game class or something (Alex)
 * 4. Refactor renderer class to be less bad (Evan)
 */
int main() {
  CustomClient c;
  c.Init("127.0.0.1", 6000);

  // initialize glfw
  glfwInit();

  // tell glfw the opengl context version
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // simple antialiasing...
  glfwWindowHint(GLFW_SAMPLES, 8);

  // create window object
  GLFWwindow* window = glfwCreateWindow(800, 600, "Game", NULL, NULL);
  if (window == NULL) {
    cout << "Failed to create window..." << endl;
    glfwTerminate();
    return -1;
  }

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  RenderManager& renderMananger = RenderManager::get();
  try {
    renderMananger.init(window);
  } catch (const std::exception& ex) {
    std::cerr << "Render Manager failed to initialize..." << std::endl;
    std::cerr << "Caused by: " << ex.what() << std::endl;
    while (1) {
    }
  }

  MeshLoader loader;
  Camera camera(glm::vec3(0, 0, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                glm::vec3(0.0f, 1.0f, 0.0));
  SceneGraphNode* sceneRoot = SceneGraphNode::getRoot();

  SceneLoader sl(ASSET("scene.json"), loader);

  // float deg = 90.0f;
  // test: move along circle
  glm::vec3 pos = glm::vec3(1.0, 0.0, 0.0);
  float angle = 0.0f;

  // the amount of time between two updates (to get 30 updates per second)
  const double UPDATE_DELTA = 1.0 / 30.0;
  // the time of the last update
  double lastUpdate = 0;

  while (!glfwWindowShouldClose(window)) {
    double now = glfwGetTime();
    double deltaTime = now - lastUpdate;

    // if enough time has passed for an update, do one
    if (deltaTime >= UPDATE_DELTA) {
      // Update Logic
      if (c.Update()) {
        break;
      }

      // deg += 0.001f;
      angle += 360.0f / (10) * UPDATE_DELTA;

      lastUpdate = now;
    }

    // Render Logic
    renderMananger.beginRender();

    camera.use();

    // Loop through every child of the root and draw them
    // Note: This only draws the first layer of the scene graph
    for (SceneGraphNode* child : sceneRoot->getChildren()) {
      // cout << child->getTransform()->getTranslation().x << endl;
      // child->getMesh()->draw(loader);
      renderMananger.draw(*child->getMesh());
      child->getMesh()->transform().setRotation(glm::vec3(90.0f, 0, 0));
      child->getMesh()->transform().setTranslation(
          glm::vec3(cos(glm::radians(angle)), sin(glm::radians(angle)), 0));
    }

    // cubeOneTransform.addRotation(glm::vec3(0.0f, deg, 0.0f));
    // cubeTwoTransform.addRotation(glm::vec3(0.0f, -deg, 0.0f));

    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  renderMananger.teardown();

  glfwTerminate();
}
