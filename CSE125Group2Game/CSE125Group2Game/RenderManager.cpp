#include "RenderManager.h"

#include "glad/glad.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define FOVY 90.0f
#define NEAR_CLIP 0.1f
#define FAR_CLIP 100.0f

RenderManager* RenderManager::instance;

RenderManager* RenderManager::get() {
  if (!instance) {
    instance = new RenderManager();
  }

  return instance;
}

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

  if (!createShader()) {
    return false;
  }

  mProjection = glm::perspective(glm::radians(FOVY), (float)width / height, NEAR_CLIP, FAR_CLIP);

  return true;
}

void RenderManager::teardown() {
  glDeleteProgram(mShaderProgram);
}

void RenderManager::beginRender() {
    glClearColor(0.2f, 0.4f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(mShaderProgram);

    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(mProjection));
}

void RenderManager::setWindowSize(int width, int height) {
  glViewport(0, 0, width, height);
  mProjection = glm::perspective(glm::radians(FOVY), (float)width / height, NEAR_CLIP, FAR_CLIP);
}

bool RenderManager::createShader() {
  // TODO: move shaders into separate folder later ....
  
  // read the vertex shader from file
  std::ifstream vert_file("vertex.glsl");
  std::string vert_shader((std::istreambuf_iterator<char>(vert_file)), std::istreambuf_iterator<char>());
  const char *vert_c_str = vert_shader.c_str();

  unsigned int vshdr;
  vshdr = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vshdr, 1, &vert_c_str, NULL);
  glCompileShader(vshdr);

  // check if compilation was successful, and print error if not
  int success;
  char infoLog[512];
  glGetShaderiv(vshdr, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vshdr, 512, NULL, infoLog);
    std::cerr << "Shader Compilation Failed\n" << infoLog << std::endl;
    return false;
  }

  // read the fragment shader from file
  std::ifstream frag_file("fragment.glsl");
  std::string frag_shader((std::istreambuf_iterator<char>(frag_file)), std::istreambuf_iterator<char>());
  const char *frag_c_str = frag_shader.c_str();

  unsigned int fshdr;
  fshdr = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fshdr, 1, &frag_c_str, NULL);
  glCompileShader(fshdr);

  // check if compilation was successful, and print error if not
  glGetShaderiv(fshdr, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fshdr, 512, NULL, infoLog);
    std::cerr << "Shader Compilation Failed\n" << infoLog << std::endl;
    return false;
  }

  // link the shader program
  mShaderProgram = glCreateProgram();
  glAttachShader(mShaderProgram, vshdr);
  glAttachShader(mShaderProgram, fshdr);
  glLinkProgram(mShaderProgram);

  glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(mShaderProgram, 512, NULL, infoLog);
    std::cerr << "Shader Link Failed\n" << infoLog << std::endl;
    return false;
  }

  // delete shader objects, unnecessary now
  glDeleteShader(vshdr);
  glDeleteShader(fshdr);

  return true;
}
