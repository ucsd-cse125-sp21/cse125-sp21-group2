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
  mLoader = new MeshLoader();
  RenderManager& renderMananger = RenderManager::get();
  renderMananger.init(mWindow);
  SceneLoader sl("../Shared/scene.json", *mLoader);
}

GameManager* GameManager::getManager() {
  if (!mManager) {
    // initialize glfw
    glfwInit();

    // tell glfw the opengl context version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_SAMPLES, 8);
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

  mPlayerTransform = &playerTransform;

  SceneGraphNode playerNode(mSceneRoot, &playerTransform,
                            Model::Cube(&playerTransform, *mLoader));

  // TODO: Turn player into gameobject

  DWORD before, after, diff;

  while (!glfwWindowShouldClose(mWindow)) {
    before = GetTickCount();
    // 1) Update local states (use key logger to update gameobject)

    glfwPollEvents();

    /*int vsp = (glfwGetKey(mWindow, GLFW_KEY_W) != 0) -
              (glfwGetKey(mWindow, GLFW_KEY_S) != 0);

    int hsp = (glfwGetKey(mWindow, GLFW_KEY_D) != 0) -
              (glfwGetKey(mWindow, GLFW_KEY_A) != 0);

    glm::vec3 velocity = glm::vec3(hsp, vsp, 0);
    if (hsp != 0 || vsp != 0)
      velocity = glm::vec3(0.5) * glm::normalize(velocity);

    playerTransform.addTranslation(velocity);*/

    bool keysPressed[NUM_KEYS];
    keysPressed[GameObject::KEY_W] = glfwGetKey(mWindow, GLFW_KEY_W);
    keysPressed[GameObject::KEY_A] = glfwGetKey(mWindow, GLFW_KEY_A);
    keysPressed[GameObject::KEY_S] = glfwGetKey(mWindow, GLFW_KEY_S);
    keysPressed[GameObject::KEY_D] = glfwGetKey(mWindow, GLFW_KEY_D);

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
      RenderManager::get().draw(*child->getMesh());
    }

    glfwSwapBuffers(mWindow);

    after = GetTickCount();

    diff = after - before;
    if (diff <= 33) Sleep(33 - diff);
  }

  RenderManager::get().teardown();
  glfwTerminate();
}

bool GameManager::UpdateObject(GameObject* obj, SceneGraphNode* node) {
  // if (obj->getName() == node->getName()) {
  //}

  mPlayerTransform->setTranslation(obj->getTransform()->getTranslation());
  return true;
}
GameObject* GameManager::Unmarshal(char* data) {
  // 1) 8 byte char[] name
  // 32 bit (4 byte) floats
  // 2) Transform: 3 floats for location, 3 for rotation, 3 for scale
  // 3) 4 byte int health 48 bytes

  char* tmpInfo = data;

  char name[NAME_LEN];
  memcpy(name, tmpInfo, NAME_LEN);  // Copy name into data
  tmpInfo += NAME_LEN;

  GLfloat xPos, yPos, zPos;
  memcpy(&xPos, tmpInfo, FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;
  memcpy(&yPos, tmpInfo, FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;
  memcpy(&zPos, tmpInfo, FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;

  // TODO: In unmarshling, euler angles -> quat
  GLfloat xRot, yRot, zRot;
  memcpy(&xRot, tmpInfo, FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;
  memcpy(&yRot, tmpInfo, FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;
  memcpy(&zRot, tmpInfo, FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;

  GLfloat xScale, yScale, zScale;
  memcpy(&xScale, tmpInfo, FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;
  memcpy(&yScale, tmpInfo, FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;
  memcpy(&zScale, tmpInfo, FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;

  int health;
  memcpy(&health, tmpInfo, INT_SIZE);

  Transform* transform =
      new Transform(glm::vec3(xPos, yPos, zPos), glm::vec3(xRot, yRot, zRot),
                    glm::vec3(xScale, yScale, zScale));

  GameObject* obj = new GameObject(transform, name, health);

  return obj;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mods) {
  // Need synchronization?
  // Suggested by Edward

  // printf("%d %d\n", key, action);

  if (action == GLFW_REPEAT) {
    return;
  }

  switch (key) {
    case GLFW_KEY_W:
      GameManager::getManager()->mKeyPresses[GameObject::KEY_W] =
          action == GLFW_PRESS;
      break;
    case GLFW_KEY_A:
      GameManager::getManager()->mKeyPresses[GameObject::KEY_A] =
          action == GLFW_PRESS;
      break;
    case GLFW_KEY_S:
      GameManager::getManager()->mKeyPresses[GameObject::KEY_S] =
          action == GLFW_PRESS;
      break;
    case GLFW_KEY_D:
      GameManager::getManager()->mKeyPresses[GameObject::KEY_D] =
          action == GLFW_PRESS;
      break;
    case GLFW_KEY_SPACE:
      break;
  }
}
