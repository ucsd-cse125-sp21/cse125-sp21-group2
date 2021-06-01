#include "GameManager.h"

#include "ConeParticleEmitter.h"
#include "Utils.h"
#include "client_helper.h"

#define SCENE_JSON "../Shared/scene.json"
#define CLIENT_CONFIG_ERROR \
  "client couldn't read config file, using default values"
// Predefinitions to make compiler happy
GameManager* GameManager::mManager;
std::string GameManager::playerModels[] = {PLAYER_MODEL_RED, PLAYER_MODEL_GREEN,
                                           PLAYER_MODEL_BLUE,
                                           PLAYER_MODEL_YELLOW};

std::string GameManager::playerColors[] = { "Red", "Green",
                                           "Blue",
                                           "yelow" };

std::string GameManager::pickupModels[] = {
    DAMAGE_BOOST_MODEL, SPEED_BOOST_MODEL,      INVINCIBILITY_MODEL,
    EXPLOSION_MODEL,    DAMAGE_REDUCTION_MODEL, SPEED_REDUCTION_MODEL,
    NO_SHOOTING_MODEL,  WEAKNESS_MODEL};

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

  // mpRenderManager->setRenderBoundingBoxes(true);

  mSound = new Sound();
  mSound->playBackgroundMusic(mSound->backgroundMusicPath);

  mpFont = new Font(ASSET("fonts/galaxy.otf"), mTLoader);
  mEndInfo = new GameEndInfo();
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
    GLFWwindow* window = glfwCreateWindow(1800, 900, "Game", NULL, NULL);
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

  auto tex = mTLoader.loadTexture(ASSET("flame.png"));
  ParticleEmitter testEmitter(tex);
  float last = glfwGetTime();

  // setup bearl different models
  // TODO(evan): make event based?
  SceneGraphNode* node = mScene.getByName("towrbear");
  node->getObject()->setModelIndexCallback([](const GameObject& obj) {
    if (obj.getHealth() > 50) {
      return 0;
    } else {
      return 1;
    }
  });

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
    mScene.Update(delta);

    // render text ui
    renderUI();

    glfwSwapBuffers(mWindow);
  }

  glfwTerminate();
}

void GameManager::renderUI() {
  // Show game over screen
  if (!mClientConnected) {
    mpRenderManager->drawText("Connecting to Server...", 275.0f, 300.0f, 0.6f,
                              glm::vec3(0.7f), *mpFont);
    return;
  }

  if (mGameOver) {
    renderGameOverUI();
  }

  // render images second
  Texture tex = mTLoader.loadTexture(ASSET("leet.png"));
  Image image(*mLoader, glm::vec2(500, 500), glm::vec2(100, 50), tex);
  // mpRenderManager->drawImage(image);

  // I <3 lambdas - Evan :)
  auto drawHealthBar = [&](const std::string& towerName,
                           const std::string& readableName, int offset) {
    // render tower health bars
    const unsigned int y_offset_amt = 20;
    GameObject* obj = mScene.getByName(towerName)->getObject();
    glm::vec3 textColor;
    if (obj->isDead()) {
      textColor = glm::vec3(1.0, 0, 0);
    } else {
      Rect healthBar(
          *mLoader, glm::vec2(80, 25 + y_offset_amt * offset),
          glm::vec2(((float)obj->getHealth() / (float)DEFAULT_HEALTH) * 150, 5),
          glm::vec4(1.0, 0, 0, 1.));
      mpRenderManager->drawRect(healthBar);
      textColor = glm::vec3(0.7);
    }
    mpRenderManager->drawText(readableName, 5.0f,
                              25 + y_offset_amt * offset - 3, 0.3f, textColor,
                              *mpFont);
  };

  drawHealthBar("towrbear", "Bearl", 0);
  drawHealthBar("towrstar", "Falling Star", 1);
  drawHealthBar("towrston", "Stone Henge", 2);
  drawHealthBar("towrsung", "Sun God", 3);

  // easter egg
  if (mEasterMode) {
    tex = mTLoader.loadTexture(ASSET("voelker.png"));
    Image voelker(*mLoader, glm::vec2(575, 20), glm::vec2(50, 50), tex);
    mpRenderManager->drawImage(voelker);
  }

  // render text third
  if (mPlayer) {
    mpRenderManager->drawText(
        "Health: " + std::to_string(mPlayer->getHealth()) + " / " +
            std::to_string(DEFAULT_HEALTH),
        25.f, 550.0f, 0.5f, glm::vec3(0.7f), *mpFont);
  }

  if (mStartGame) {
    if (mWaveTimer) {
      mpRenderManager->drawText("Next Wave: " + std::to_string(mWaveTimer),
                                650.0f, 25.0f, 0.5f, glm::vec3(0.7f), *mpFont);
    } else {
      std::string enemiesLeft = "Enemies Left: " + std::to_string(mNumEnemies);
      mpRenderManager->drawText(enemiesLeft, 350.0f, 555.0f, 0.4f,
                                glm::vec3(0.7f), *mpFont);
    }

    mpRenderManager->drawText("Wave: " + std::to_string(mWavesCompleted),
                              650.0f, 550.0f, 0.5f, glm::vec3(0.7f), *mpFont);
  } else {
    if (!mReady) {
      mpRenderManager->drawText("Press enter to start", 275.0f, 25.0f, 0.6f,
                                glm::vec3(0.7f), *mpFont);
    } else {
      mpRenderManager->drawText("Waiting for players...", 275.0f, 550.0f, 0.6f,
                                glm::vec3(0.7f), *mpFont);
      mpRenderManager->drawText(
          std::to_string(mCurrPlayers) + "/" + std::to_string(mMinPlayers),
          350.0f, 500.0f, 0.6f, glm::vec3(0.7f), *mpFont);
    }
  }
}

void GameManager::renderGameOverUI() {
  // render rects first,
  Rect rect(*mLoader, glm::vec2(0, 0), glm::vec2(800, 600),
            glm::vec4(1.0, 0, 0, 0.5));
  mpRenderManager->drawRect(rect);

  mpRenderManager->drawText("GAME OVER", 300.0f, 400.0f, 1.0f,
                            glm::vec3(1.0f, 0, 0), *mpFont);
  mpRenderManager->drawText(
      "Time Ellapsed: " + std::to_string(mEndInfo->timeEllapsed/1000) + "s", 320.0f,
      350.0f, 0.5f, glm::vec3(1.0f), *mpFont);
  mpRenderManager->drawText(
      "You Survived " + std::to_string(mEndInfo->highScore) + " Wave(s)", 285.0f, 315.0f,
      0.5f, glm::vec3(1.0f), *mpFont);
  mpRenderManager->drawText(
      "Player Name" , 230.0f, 280.0f,
      0.5f, glm::vec3(0.7f), *mpFont);
  mpRenderManager->drawText(
      "Kills", 400.0f, 280.0f,
      0.5f, glm::vec3(0.7f), *mpFont);
  mpRenderManager->drawText(
      "Downs", 488.0f, 280.0f,
      0.5f, glm::vec3(0.7f), *mpFont);
  std::vector<int> enemiesKilledPerPlayer = mEndInfo->mEnemiesKilledPerPlayer;
  std::vector<int> numRespawnedPerPlayer = mEndInfo->mNumRespawnedPerPlayer;

  /*std::vector<int> enemiesKilledPerPlayer = { 1,2,3 };
  std::vector<int> numRespawnedPerPlayer = {1,2,3};*/
  for (int i = 0; i < enemiesKilledPerPlayer.size(); i++) {
      mpRenderManager->drawText(
          "Player" + std::to_string(i) + " (" + playerColors[i] + ")", 230.0f, 280.0f - ((i+1)*40),
          0.5f, glm::vec3(1.0f), *mpFont);
      mpRenderManager->drawText(
          std::to_string(enemiesKilledPerPlayer[i]), 400.0f, 280.0f - ((i + 1) * 40),
          0.5f, glm::vec3(1.0f), *mpFont);
      mpRenderManager->drawText(
          std::to_string(numRespawnedPerPlayer[i]), 488.0f, 280.0f - ((i + 1) * 40),
          0.5f, glm::vec3(1.0f), *mpFont);
      if (mEndInfo->mvpPlayerID == i) {
          mpRenderManager->drawText(
              "MVP", 570.0f, 280.0f - ((i + 1) * 40),
              0.5f, glm::vec3(1.0f), *mpFont);
      }
  }
}

void GameManager::updateKeyPresses(bool* keysPressed) {
  keysPressed[FORWARD] = glfwGetKey(mWindow, FORWARD_KEY);
  keysPressed[LEFT] = glfwGetKey(mWindow, LEFT_KEY);
  keysPressed[BACKWARD] = glfwGetKey(mWindow, BACKWARD_KEY);
  keysPressed[RIGHT] = glfwGetKey(mWindow, RIGHT_KEY);
  keysPressed[SHOOT] = glfwGetKey(mWindow, PROJECTILE_KEY);
  keysPressed[RESTART] = glfwGetKey(mWindow, RESTART_KEY);
  keysPressed[READY] = glfwGetKey(mWindow, READY_KEY);

  if (keysPressed[READY]) {
    mReady = true;
  }
  mEasterMode = glfwGetKey(mWindow, EASTER_KEY) == GLFW_PRESS ||
                glfwGetKey(mWindow, EASTER_KEY) == GLFW_REPEAT;
}

void GameManager::UpdateObject(GameObject* obj) {
  SceneGraphNode* foundNode = mScene.getByName(obj->getName());
  GameObject* foundObject;

  // Object does not exist, create it
  if (!foundNode) {
    spawnObject(obj, foundObject, foundNode);
  }

  // Update object
  foundObject = foundNode->getObject();

  // Health is 0, delete object
  if (obj->isDead()) {
    foundObject->setHealth(0);
    // Don't render the player if they die
    if (foundObject->isPlayer()) {
      foundObject->mShouldRender = false;
      return;
    } else if (obj->isTower()) {
      foundObject->mShouldRender = false;
      // Play tower Destroyed sound
      irrklang::vec3d position(obj->getTransform()->getTranslation().x,
                               obj->getTransform()->getTranslation().y,
                               obj->getTransform()->getTranslation().z);
      mSound->play(mSound->towerCollapseSoundPath, position);

      return;
    } else if (foundObject->isEnemy()) {
      foundObject->mShouldRender = false;
      Texture flameTexture = mTLoader.loadTexture(ASSET("flame.png"));
      foundNode->emitter = new ParticleEmitter(flameTexture);
      mNumEnemies--;
      return;
    }
    // std::cerr << "Deleting object: " << ((std::string)obj->getName())
    //        << std::endl;
    mSound->deleteFromSoundVector(obj);
    mScene.removeChild(foundNode);
    return;
  } else {
    foundObject->mShouldRender = true;
  }

  updateSound(foundObject);

  foundObject->getTransform()->setModel(obj->getTransform()->getModel());
  foundObject->setHealth(obj->getHealth());
}

void GameManager::updateSound(GameObject* foundObject) {
  // Update sound listener position on player update
  if (mPlayer && (foundObject == mPlayer)) {
    // 3) Update sound listener position
    mSound->setListenerPosition(mPlayer->getTransform());
  }
}

void GameManager::spawnObject(GameObject* obj, GameObject*& foundObject,
                              SceneGraphNode*& foundNode) {
  Transform* transform = new Transform(
      obj->getTransform()->getTranslation(), obj->getTransform()->getRotation(),
      obj->getTransform()->getScale(), obj->getTransform()->getBBoxLens());

  transform->setModel(obj->getTransform()->getModel());
  // std::cout << "creating new  object" << std::endl;

  foundObject = new GameObject(transform, obj->getName(), obj->getHealth(),
                               obj->getObjectType());

  Model* model = nullptr;
  if (obj->isEnemy()) {
    model = mMLoader.LoadModel(ENEMY_MODEL, *mLoader, mTLoader);
  } else if (obj->isPlayer()) {
    model = mMLoader.LoadModel(playerModels[obj->getPlayerId()], *mLoader,
                               mTLoader);

    // If this is the first time a player connects, add it!
    if ((obj->getName() == GameObject::makeName("play", mClientId))) {
      addPlayer(foundObject, model);
    }
  } else if (obj->isProjectile()) {
    model = mMLoader.LoadModel(PROJECTILE_MODEL, *mLoader, mTLoader);
  } else if (obj->isPickup()) {
    std::cout << "Model IDX" << obj->mModifier - 1 << std::endl;
    model = mMLoader.LoadModel(pickupModels[obj->mModifier - 1], *mLoader,
                               mTLoader);
  } else if (!obj->isTower()) {
    model = Model::Cube(*mLoader);
  }

  foundNode = mScene.addChild(foundObject, model);
}

void GameManager::addPlayer(GameObject*& foundObject, Model* model) {
  mPlayer = foundObject;

  SceneGraphNode* playerNode = mScene.addChild(foundObject, model);

  // attach the camera to the player
  Camera& camera = mScene.addCamera(playerNode);
  camera.setPosition(glm::vec3(0, 30.0f, 0));
  camera.setFacing(glm::vec3(0, 0, 0));
  camera.setUp(glm::vec3(0.0f, 0, -1.0f));

  // attach emitters to player
  GameObject* bsObj1 =
      new GameObject(new Transform(glm::vec3(2.0, 0.0, 3.0), glm::vec3(0),
                                   glm::vec3(0.2, 0.2, 0.2)),
                     "bsObj1", 100);
  SceneGraphNode* emitter1 = mScene.addChild(bsObj1, nullptr, playerNode);
  Texture flameTexture = mTLoader.loadTexture(ASSET("flame.png"));
  emitter1->emitter =
      new ConeParticleEmitter(flameTexture, glm::vec3(0, 0, 1), 30, 200);
  emitter1->emitter->mIsContinuous = true;
  emitter1->emitter->mParticleSize = 1.0;
  emitter1->emitter->mParticleSpeed = 3.0;
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

  int modifier;
  memcpy(&modifier, tmpInfo, INT_SIZE);
  tmpInfo += INT_SIZE;

  Transform* transform = new Transform(glm::vec3(0), glm::vec3(0), glm::vec3(0),
                                       glm::vec3(xbb, ybb, zbb));

  transform->setModel(model);
  // TODO: should we add forward vector on client?
  GameObject* obj = new GameObject(transform, name, health, type);
  obj->mModifier = modifier;

  mSound->playAccordingToGameObject(obj);

  return obj;
}

void GameManager::setClientID(int id) { mClientId = id; }

void GameManager::ResizeCallback(int width, int height) {
  mpRenderManager->setViewportSize(width, height);
}
