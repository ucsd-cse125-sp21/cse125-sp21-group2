#pragma once

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

// singleton
/* Responsibilities:
 * - Initializing opengl
 * - Shader compilation/management
 */
class RenderManager
{ 
public:
  static RenderManager* get();
  bool init(GLFWwindow* window);
  void teardown();

  void beginRender();

  void setWindowSize(int width, int height);

private:
  static RenderManager* instance;

  unsigned int mShaderProgram;
  glm::mat4 mProjection;  

  bool createShader();
};

