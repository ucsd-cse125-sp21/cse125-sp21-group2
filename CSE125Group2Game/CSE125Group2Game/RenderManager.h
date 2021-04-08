#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <memory>

#include "ShaderProgram.h"

// singleton
/* Responsibilities:
 * - Initializing opengl
 * - Shader compilation/management
 */
class RenderManager {
 public:
  static RenderManager& get();
  bool init(GLFWwindow* window);
  void teardown();

  void beginRender();

  void setViewportSize(int width, int height);

 private:
  // for now, just support one shader program...
  // unique_ptr is just a smart pointer that will automatically call the
  // destructor when out of scope... see
  // https://en.cppreference.com/w/cpp/memory/unique_ptr
  std::unique_ptr<ShaderProgram> mpShaderProgram;
  glm::mat4 mProjection = glm::mat4(1.0f);
};
