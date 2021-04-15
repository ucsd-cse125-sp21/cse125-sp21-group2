#include "GameManager.h"

#include "client_helper.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  RenderManager::get().setViewportSize(width, height);
}

GameManager::GameManager() {
  // initialize glfw
  glfwInit();

  // tell glfw the opengl context version
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  mCamera = new Camera(glm::vec3(0, 0, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                       glm::vec3(0.0f, 1.0f, 0.0));
}

void GameManager::Update() {
  std::string host = DEFAULT_SERVER_HOST;
  uint16_t port = DEFAULT_SERVER_PORT;
  std::string filename = CONFIG_FILE;

  if (!read_client_config(host, port, filename)) {
    std::cout
        << "client couldn't read config file, using default host and port";
  }

  std::cout << "host: " << host << "port: " << port << "\n";

  CustomClient c;
  c.Init(host, port);

  // create window object
  GLFWwindow* window = glfwCreateWindow(800, 600, "Game", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create window..." << std::endl;
    glfwTerminate();
    return;
  }

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  RenderManager& renderMananger = RenderManager::get();
  renderMananger.init(window);

  SceneGraphNode* sceneRoot = SceneGraphNode::getRoot();

  SceneLoader sl(ASSET("scene.json"));

  // Create player and set it as first layer (child of root)
  Transform playerTransform(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0),
                            glm::vec3(1, 1, 1));
  Mesh* playerMesh = Mesh::Cube(&playerTransform);
  SceneGraphNode playerNode(sceneRoot, &playerTransform, playerMesh);

  Keyboard keyboard(window);

  float deg = 0.0f;

  while (!glfwWindowShouldClose(window)) {
    // 1) Update local states (use key logger to update gameobject)
    keyboard.poll();

    // 2) Call client update
    if (c.Update()) {
      break;
    }

    // 3) Call drawAll on scene graph
    renderMananger.beginRender();
    mCamera->use();

    // Loop through every child of the root and draw them
    // Note: This only draws the first layer of the scene graph
    for (SceneGraphNode* child : sceneRoot->getChildren()) {
      // cout << child->getTransform()->getTranslation().x << endl;
      child->getMesh()->draw();
    }

    deg += 0.001f;
    playerTransform.addRotation(glm::vec3(0, deg, 0));

    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  renderMananger.teardown();

  glfwTerminate();
}
