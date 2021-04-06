// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define GLFW_INCLUDE_NONE

#include <iostream>

#include <GLFW/glfw3.h>

#include "RenderManager.h"
#include "Mesh.h"
#include "Camera.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  RenderManager::get()->setWindowSize(width, height);
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

  RenderManager* renderMananger = RenderManager::get();
  renderMananger->init(window);

  Transform transform(glm::vec3(0,0,0), glm::vec3(0,0,0), glm::vec3(1,1,1));

  Mesh cube = Mesh::Cube(&transform);
  Camera camera(glm::vec3(0, 0, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0));

  float deg = 0.0f;
  while (!glfwWindowShouldClose(window)) {
    // draw all the things
    renderMananger->beginRender();
  
    camera.use();

    cube.draw();
    transform.addRotation(glm::vec3(0.0f, deg, 0.0f));
    transform.setScale(glm::vec3(0.1, 0.1, 0.1));
    //transform.setTranslation(glm::vec3(5,0,0));

    deg += 0.1f;

    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  renderMananger->teardown();

  glfwTerminate();
}
