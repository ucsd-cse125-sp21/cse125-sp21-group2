// main.cpp : This file contains the 'main' function. Program execution begins
// and ends there.
//

#include <GLFW/glfw3.h>

#include <iostream>

#include "Camera.h"
#include "Mesh.h"
#include "RenderManager.h"
#include "SceneGraphNode.h"
#include "SceneLoader.h"
#include "Transform.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  RenderManager::get().setViewportSize(width, height);
}

int main() {
  // initialize glfw
  glfwInit();

  // tell glfw the opengl context version
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // create window object
  GLFWwindow* window = glfwCreateWindow(800, 600, "Game", NULL, NULL);
  if (window == NULL) {
    cout << "Failed to create window..." << endl;
    glfwTerminate();
    return -1;
  }

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  RenderManager& renderMananger = RenderManager::get();
  renderMananger.init(window);

  Camera camera(glm::vec3(0, 0, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                glm::vec3(0.0f, 1.0f, 0.0));
  SceneGraphNode* sceneRoot = SceneGraphNode::getRoot();

  // Transform cubeOneTransform(glm::vec3(5, 0, 0), glm::vec3(0, 0, 0),
  // glm::vec3(1, 1, 1));
  // Mesh cubeOne = Mesh::Cube(&cubeOneTransform);

  // Transform cubeTwoTransform(glm::vec3(-5, 0, 0), glm::vec3(10, 50, 4),
  // glm::vec3(1, 1, 1));
  // Mesh cubeTwo = Mesh::Cube(&cubeTwoTransform);

  // SceneGraphNode* cubeOneNode =
  //  new SceneGraphNode(sceneRoot, &cubeOneTransform, &cubeOne);
  // SceneGraphNode* cubeTwoNode =
  //  new SceneGraphNode(sceneRoot, &cubeTwoTransform, &cubeTwo);

  // Mesh cube = Mesh("tree.obj");
  // Mesh cube = Mesh::Cube();
  // Camera camera(glm::vec3(0, 0, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f),
  // glm::vec3(0.0f, 1.0f, 0.0));

  SceneLoader sl("scene.json");

  float deg = 0.0f;

  while (!glfwWindowShouldClose(window)) {
    // draw all the things

    renderMananger.beginRender();

    camera.use();

    // Loop through every child of the root and draw them
    // Note: This only draws the first layer of the scene graph
    for (SceneGraphNode* child : sceneRoot->getChildren()) {
      // cout << child->getTransform()->getTranslation().x << endl;
      child->getMesh()->draw();
    }

    // cubeOneTransform.addRotation(glm::vec3(0.0f, deg, 0.0f));
    // cubeTwoTransform.addRotation(glm::vec3(0.0f, -deg, 0.0f));

    deg += 0.001f;

    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  renderMananger.teardown();

  glfwTerminate();
}
