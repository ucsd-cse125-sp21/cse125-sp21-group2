#include "GameManager.h"

#include "Utils.h"
#include "client_helper.h"

// Predefinitions to make compiler happy
GameManager* GameManager::mManager;
void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mods);

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  GameManager::getManager()->ResizeCallback(width, height);
}

GameManager::GameManager(GLFWwindow* window) : mWindow(window) {
  mCamera = new Camera(glm::vec3(0, 0, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                       glm::vec3(0.0f, 1.0f, 0.0));

  mLoader = new MeshLoader();
  mpRenderManager =
      std::make_unique<RenderManager>(window, *mLoader, mTLoader, mCamera);
  // RenderManager& renderMananger = RenderManager::get();
  // renderMananger.init(mWindow);
  glfwSetWindowUserPointer(mWindow, this);
  mScene = SceneGraph::FromFile("../Shared/scene.json", *mLoader, mTLoader);
  mpRenderManager->setRenderBoundingBoxes(true);
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
      glfwTerminate();
      CRASH("Failed to create glfw window...");
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

  while (!glfwWindowShouldClose(mWindow)) {
    // 1) Update local states (use key logger to update gameobject)

    auto node = mScene.getByName("cube1");
    node->getObject()->getTransform()->addRotation(glm::vec3(0, 0, 0.1f));

    glfwPollEvents();

    bool keysPressed[NUM_KEYS];
    keysPressed[GameObject::FORWARD] = glfwGetKey(mWindow, FORWARD_KEY);
    keysPressed[GameObject::LEFT] = glfwGetKey(mWindow, LEFT_KEY);
    keysPressed[GameObject::BACKWARD] = glfwGetKey(mWindow, BACKWARD_KEY);
    keysPressed[GameObject::RIGHT] = glfwGetKey(mWindow, RIGHT_KEY);
    keysPressed[GameObject::SHOOT] = glfwGetKey(mWindow, PROJECTILE_KEY);

    // 2) Call client update
    if (c.Update(keysPressed)) {
      break;
    }

    // 3) Call drawAll on scene graph
    mpRenderManager->beginRender();
    mCamera->use();

    // draw scene
    mpRenderManager->draw(mScene, *mLoader);

    glfwSwapBuffers(mWindow);
  }

  glfwTerminate();
}

void GameManager::AddPlayer(int clientId) {
  // Create player and set it as first layer (child of root)
  Transform* playerTransform =
      new Transform(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1),
                    glm::vec3(0.5f, 0.5f, 0.5f));

  mPlayerTransform = playerTransform;

  std::string clientName = "play000";
  clientName += std::to_string(clientId);

  GameObject* playerObject =
      new GameObject(playerTransform, (char*)clientName.c_str(), 10);

  // TODO: make camera a child of the player object in the scene graph

  // Model* model =
  // new Model(ASSET("models/enemy/mainEnemyShip/enemyShip.obj"),
  // playerTransform, *mLoader);
  Model* model = Model::Cube(playerTransform, *mLoader);

  mScene.addChild(playerObject, model);
}

void GameManager::UpdateObject(GameObject* obj) {
  // SceneGraphNode* foundNode = findNode(obj, mScene.getRoot());
  SceneGraphNode* foundNode = mScene.getByName(obj->getName());
  GameObject* foundObject;

  // Object does not exist, create it
  if (!foundNode) {
    // std::cout << "creating new  object" << std::endl;
    foundObject =
        new GameObject(new Transform(obj->getTransform()->getTranslation(),
                                     obj->getTransform()->getRotation(),
                                     obj->getTransform()->getScale()),
                       obj->getName(), obj->getHealth());

    // TODO: if else for model based on enum (constructor adds itself as child
    // of parent)
    foundNode = mScene.addChild(
        foundObject, Model::Cube(foundObject->getTransform(), *mLoader));
  }

  // Update object
  foundObject = foundNode->getObject();

  // Health is 0, delete object
  if (obj->getHealth() <= 0) {
    // std::cerr << "Deleting object: " << ((std::string)obj->getName())
    //        << std::endl;
    mScene.removeChild(foundNode);
    return;
  }

  // Set found objects position, rotation, scale, and health to passed in obj
  foundObject->getTransform()->setTranslation(
      obj->getTransform()->getTranslation());

  foundObject->getTransform()->setRotation(obj->getTransform()->getRotation());

  foundObject->getTransform()->setScale(obj->getTransform()->getScale());

  foundObject->setHealth(obj->getHealth());
  return;
}

SceneGraphNode* GameManager::findNode(GameObject* obj, SceneGraphNode* node) {
  if (obj->getName() == node->getObject()->getName()) {
    return node;
  }

  for (int i = 0; i < node->getChildren().size(); i++) {
    SceneGraphNode* foundNode = findNode(obj, node->getChildren()[i]);

    if (foundNode) {
      return foundNode;
    }
  }

  return NULL;
}
GameObject* GameManager::Unmarshal(char* data) {
  // 1) 8 byte char[] name
  // 32 bit (4 byte) floats
  // 2) Transform: 3 floats for location, 3 for rotation, 3 for scale
  // 3) 4 byte int health 48 bytes

  char* tmpInfo = data;

  // NAME_LEN + 1 so we ensure it is null terminated
  char name[NAME_LEN + 1] = {0};
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
  tmpInfo += INT_SIZE;

  GLfloat xbb, ybb, zbb;
  memcpy(&xbb, tmpInfo, FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;
  memcpy(&ybb, tmpInfo, FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;
  memcpy(&zbb, tmpInfo, FLOAT_SIZE);
  tmpInfo += FLOAT_SIZE;

  Transform* transform = new Transform(
      glm::vec3(xPos, yPos, zPos), glm::vec3(xRot, yRot, zRot),
      glm::vec3(xScale, yScale, zScale), glm::vec3(xbb, ybb, zbb));

  // TODO: should we add forward vector on client?
  GameObject* obj = new GameObject(transform, name, health);

  return obj;
}

void GameManager::setClientID(int id) { mClientId = id; }

void GameManager::ResizeCallback(int width, int height) {
  mpRenderManager->setViewportSize(width, height);
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
    case GameManager::FORWARD_KEY:
      GameManager::getManager()->mKeyPresses[GameObject::FORWARD] =
          action == GLFW_PRESS;
      break;
    case GameManager::LEFT_KEY:
      GameManager::getManager()->mKeyPresses[GameObject::LEFT] =
          action == GLFW_PRESS;
      break;
    case GameManager::BACKWARD_KEY:
      GameManager::getManager()->mKeyPresses[GameObject::BACKWARD] =
          action == GLFW_PRESS;
      break;
    case GameManager::RIGHT_KEY:
      GameManager::getManager()->mKeyPresses[GameObject::RIGHT] =
          action == GLFW_PRESS;
      break;
    case GLFW_KEY_SPACE:
      break;
  }
}
