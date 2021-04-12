#include "RenderManager.h"

#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>

#define FOVY 90.0f
#define NEAR_CLIP 0.1f
#define FAR_CLIP 100.0f

RenderManager& RenderManager::get() {
  static RenderManager instance;
  return instance;
}

/**
 * Initializes the RenderManager using the passed in window as the render
 * target.
 *
 * This method should only be called once.
 *
 * @param window The window to use as a render target.
 */
bool RenderManager::init(GLFWwindow* window) {
  // tell opengl to use the current window context
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to init glad ..." << std::endl;
    return false;
  }

  // ensure we cull the back face to prevent wacky artifacts
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnable(GL_DEPTH_TEST);

  // tell opengl our screen coordinate ranges
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

  mpShaderProgram =
      std::make_unique<ShaderProgram>("vertex.glsl", "fragment.glsl");

  mProjection = glm::perspective(glm::radians(FOVY), (float)width / height,
                                 NEAR_CLIP, FAR_CLIP);

  return true;
}

void RenderManager::teardown() {}

/**
 * Must be called before a series of render calls, to clear render state, etc.
 */
void RenderManager::beginRender() {
  glClearColor(0.2f, 0.4f, 0.4f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  mpShaderProgram->use();

  glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(mProjection));
}

/**
 * Used to set the viewport size. Should be called any time the window size
 * changes.
 *
 * @param width the width of the new viewport.
 * @param height the height of the new viewport.
 */
void RenderManager::setViewportSize(int width, int height) {
  glViewport(0, 0, width, height);

  // project matrix encodes aspect ratio...need to update it as the aspect ratio
  // has implicitly changed
  mProjection = glm::perspective(glm::radians(FOVY), (float)width / height,
                                 NEAR_CLIP, FAR_CLIP);
}
