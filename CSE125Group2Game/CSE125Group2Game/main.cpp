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

  string host = DEFAULT_SERVER_HOST;
  uint16_t port = DEFAULT_SERVER_PORT;
  std::string filename = CONFIG_FILE;

  std::string line;
  std::ifstream infile;

  infile.open(filename.c_str());
  std::vector<std::string> tokens;
  std::string intermediate;

  // read the config file and fill in port and host values, if defined
  if (infile) {
    while (getline(infile, line)) {
      std::stringstream ss(line);

      while (getline(ss, intermediate, ':')) {
        tokens.push_back(intermediate);
      }

      if (tokens[0].compare("port") == 0) {
        int temp_int(std::stoi(tokens[1]));

        if (temp_int <= static_cast<int>(UINT16_MAX) && temp_int >= 0) {
          port = static_cast<uint16_t>(temp_int);
        }
      } else if (tokens[0].compare("host") == 0) {
        cout << "host: " << tokens[1] << "\n";
        host = tokens[1];
      }
    }
  } else {
    std::cout << "couldn't open file, using default port and tick\n";
  }

  std::cout << "host: " << host << "port: " << port << "\n";
  c.Init(host, port);

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

  SceneLoader sl(ASSET("scene.json"));

  float deg = 0.0f;

  while (!glfwWindowShouldClose(window)) {
    if (c.Update()) {
      break;
    }

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
