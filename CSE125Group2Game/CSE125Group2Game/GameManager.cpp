#include "GameManager.h"

#include "client_helper.h"

// Predefinitions to make compiler happy
GameManager* GameManager::mManager;
void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mods);

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  RenderManager::get().setViewportSize(width, height);
}

GameManager::GameManager(GLFWwindow* window) {
  mCamera = new Camera(glm::vec3(0, 0, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                       glm::vec3(0.0f, 1.0f, 0.0));

  mSceneRoot = SceneGraphNode::getRoot();

  mWindow = window;
  RenderManager& renderMananger = RenderManager::get();
  renderMananger.init(mWindow);
  SceneLoader sl("../Shared/scene.json");
}

GameManager* GameManager::getManager() {
  if (!mManager) {
    // initialize glfw
    glfwInit();

    // tell glfw the opengl context version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create window object
    GLFWwindow* window = glfwCreateWindow(800, 600, "Game", NULL, NULL);
    if (window == NULL) {
      std::cout << "Failed to create window..." << std::endl;
      glfwTerminate();
      return NULL;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    mManager = new GameManager(window);
  }

  return mManager;
}

void GameManager::Update() {
  glfwSetKeyCallback(mWindow, key_callback);

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

  // Create player and set it as first layer (child of root)
  Transform playerTransform(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0),
                            glm::vec3(1, 1, 1));
  SceneGraphNode playerNode(mSceneRoot, &playerTransform,
                            Mesh::Cube(&playerTransform));

  while (!glfwWindowShouldClose(mWindow)) {
    // 1) Update local states (use key logger to update gameobject)

    glfwPollEvents();

    int vsp = (glfwGetKey(mWindow, GLFW_KEY_W) != 0) -
              (glfwGetKey(mWindow, GLFW_KEY_S) != 0);

    int hsp = (glfwGetKey(mWindow, GLFW_KEY_D) != 0) -
              (glfwGetKey(mWindow, GLFW_KEY_A) != 0);

    glm::vec3 velocity = glm::vec3(hsp, vsp, 0);
    if (hsp != 0 || vsp != 0)
      velocity = glm::vec3(0.5) * glm::normalize(velocity);

    playerTransform.addTranslation(velocity);

    bool keysPressed[NUM_KEYS];
    keysPressed[KEY_W] = glfwGetKey(mWindow, GLFW_KEY_W);
    keysPressed[KEY_A] = glfwGetKey(mWindow, GLFW_KEY_A);
    keysPressed[KEY_S] = glfwGetKey(mWindow, GLFW_KEY_S);
    keysPressed[KEY_D] = glfwGetKey(mWindow, GLFW_KEY_D);

    // 2) Call client update
    if (c.Update(keysPressed)) {
      break;
    }

    // 3) Call drawAll on scene graph
    RenderManager::get().beginRender();
    mCamera->use();

    // Loop through every child of the root and draw them
    // Note: This only draws the first layer of the scene graph
    for (SceneGraphNode* child : mSceneRoot->getChildren()) {
      // cout << child->getTransform()->getTranslation().x << endl;
      child->getMesh()->draw();
    }

    glfwSwapBuffers(mWindow);

    // resetKeys();
  }

  RenderManager::get().teardown();
  glfwTerminate();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mods) {
  // Keyboard::lock.lock();  // Hold lock to log key is pressed

  switch (key) {
    case GLFW_KEY_W:
      if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        // TODO: Callback for key
      }
      break;
    case GLFW_KEY_A:
      if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        // TODO: Callback for key
      }
      break;
    case GLFW_KEY_S:
      if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        // TODO: Callback for key
      }
      break;
    case GLFW_KEY_D:
      if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        // TODO: Callback for key
      }
      break;
    case GLFW_KEY_SPACE:
      if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        // GLFW_PRESS: For when key is press once
        // GLFW_REPEAT: For when key is held
      }
      break;
  }

  // Keyboard::lock.unlock();
}
