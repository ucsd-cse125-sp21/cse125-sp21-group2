#include "GameManager.h"

#include "Utils.h"
#include "client_helper.h"

#define SCENE_JSON "../Shared/scene.json"
#define CLIENT_CONFIG_ERROR \
  "client couldn't read config file, using default values"
// Predefinitions to make compiler happy
GameManager* GameManager::mManager;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  GameManager::getManager()->ResizeCallback(width, height);
}

GameManager::GameManager(GLFWwindow* window) : mWindow(window) {
  mLoader = new MeshLoader();
  mpRenderManager =
      std::make_unique<RenderManager>(window, *mLoader, mTLoader, mCamera);

  glfwSetWindowUserPointer(mWindow, this);
  mScene = SceneGraph::FromFile(SCENE_JSON, *mLoader, mTLoader, mMLoader);
  mScene.setSkybox(new Skybox(ASSET("skybox"), *mLoader, mTLoader));

  /*mScene.getByName("tree1")->getModel()->mMaterials[0].isRainbow = true;
  mScene.getByName("tree1")->getModel()->mMaterials[1].isRainbow = true;*/

  mpRenderManager->setRenderBoundingBoxes(true);

  mSound = new Sound();
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
  std::string host = DEFAULT_SERVER_HOST;
  uint16_t port = DEFAULT_SERVER_PORT;
  std::string filename = CONFIG_FILE;

  if (!read_client_config(host, port, filename)) {
    std::cout << CLIENT_CONFIG_ERROR;
  }

  std::cout << "host: " << host << "port: " << port << "\n";

  CustomClient c;
  c.Init(host, port);

  ParticleEmitter testEmitter;
  float last = glfwGetTime();

  while (!glfwWindowShouldClose(mWindow)) {
    // 1) Update local states (use key logger to update gameobject)
    float now = glfwGetTime();
    float delta = now - last;
    last = now;

    glfwPollEvents();

    bool keysPressed[NUM_KEYS];
    updateKeyPresses(keysPressed);

    // 2) Call client update
    if (c.Update(keysPressed)) {
      break;
    }

    // 4) Call drawAll on scene graph
    mpRenderManager->beginRender();

    // draw scene
    mpRenderManager->draw(mScene, *mLoader);
    mpRenderManager->draw(testEmitter, mScene);
    testEmitter.Update(delta);

    glfwSwapBuffers(mWindow);
  }

  glfwTerminate();
}

void GameManager::updateKeyPresses(bool* keysPressed) {
  keysPressed[FORWARD] = glfwGetKey(mWindow, FORWARD_KEY);
  keysPressed[LEFT] = glfwGetKey(mWindow, LEFT_KEY);
  keysPressed[BACKWARD] = glfwGetKey(mWindow, BACKWARD_KEY);
  keysPressed[RIGHT] = glfwGetKey(mWindow, RIGHT_KEY);
  keysPressed[SHOOT] = glfwGetKey(mWindow, PROJECTILE_KEY);
  keysPressed[RESTART] = glfwGetKey(mWindow, RESTART_KEY);
}

void GameManager::UpdateObject(GameObject* obj) {
  // SceneGraphNode* foundNode = findNode(obj, mScene.getRoot());
  SceneGraphNode* foundNode = mScene.getByName(obj->getName());
  GameObject* foundObject;

  // Object does not exist, create it
  if (!foundNode) {
    Transform* transform = new Transform(obj->getTransform()->getTranslation(),
                                         obj->getTransform()->getRotation(),
                                         obj->getTransform()->getScale(),
                                         obj->getTransform()->getBBox());

    transform->setModel(obj->getTransform()->getModel());
    // std::cout << "creating new  object" << std::endl;

    foundObject = new GameObject(transform, obj->getName(), obj->getHealth(),
                                 obj->getObjectType());

    Model* model = nullptr;
    if (obj->isTower()) {
      model = mMLoader.LoadModel(STONEHENGE_MODEL, *mLoader, mTLoader);
    } else if (obj->isEnemy()) {
      model = mMLoader.LoadModel(PLAYER_MODEL, *mLoader, mTLoader);
    } else if (obj->isPlayer()) {
      model = mMLoader.LoadModel(PLAYER_MODEL, *mLoader, mTLoader);

      // If this is the first time a player connects, add it!
      if (obj->getName() == GameObject::makeName("play", mClientId)) {
        mPlayerTransform = foundObject->getTransform();

        SceneGraphNode* playerNode = mScene.addChild(foundObject, model);

        // attach the camera to the player
        Camera& camera = mScene.addCamera(playerNode);
        camera.setPosition(glm::vec3(0, 20.0f, 0));
        camera.setFacing(glm::vec3(0, 0, 0));
        camera.setUp(glm::vec3(0.0f, 0, -1.0f));
      }
    } else {
      model = Model::Cube(*mLoader);
    }
    foundNode = mScene.addChild(foundObject, model);
  }

  // Update object
  foundObject = foundNode->getObject();

  // Health is 0, delete object
  if (obj->isDead()) {
    // Don't render the player if they die
    if (foundObject->isPlayer() || obj->isTower()) {
      foundObject->mShouldRender = false;
      return;
    }

    // std::cerr << "Deleting object: " << ((std::string)obj->getName())
    //        << std::endl;
    mScene.removeChild(foundNode);
    return;
  } else {
    foundObject->mShouldRender = true;
  }

  // Update sound listener position on player update
  if (foundObject->getTransform() == mPlayerTransform) {
    // 3) Update sound listener position
    mSound->setListenerPosition(mPlayerTransform);
  }

  foundObject->getTransform()->setModel(obj->getTransform()->getModel());
}

GameObject* GameManager::unmarshalInfo(char* data) {
  char* tmpInfo = data;

  // NAME_LEN + 1 so we ensure it is null terminated
  char name[NAME_LEN + 1] = {0};
  memcpy(name, tmpInfo, NAME_LEN);  // Copy name into data
  tmpInfo += NAME_LEN;

  glm::mat4 model;

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      memcpy(&(model[j][i]), tmpInfo, FLOAT_SIZE);
      tmpInfo += FLOAT_SIZE;
    }
  }

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

  ObjectType type;
  memcpy(&type, tmpInfo, TYPE_SIZE);
  tmpInfo += TYPE_SIZE;

  Transform* transform = new Transform(glm::vec3(0), glm::vec3(0), glm::vec3(0),
                                       glm::vec3(xbb, ybb, zbb));

  transform->setModel(model);
  // TODO: should we add forward vector on client?
  GameObject* obj = new GameObject(transform, name, health, type);

  mSound->playAccordingToGameObject(obj);

  return obj;
}

void GameManager::setClientID(int id) { mClientId = id; }

void GameManager::ResizeCallback(int width, int height) {
  mpRenderManager->setViewportSize(width, height);
}
