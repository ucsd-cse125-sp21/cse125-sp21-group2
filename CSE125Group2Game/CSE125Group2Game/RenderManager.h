﻿#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <memory>

#include "MeshLoader.h"
#include "Model.h"
#include "SceneGraph.h"
#include "ShaderProgram.h"

// singleton
/* Responsibilities:
 * - Initializing opengl
 * - Shader compilation/management
 */
class RenderManager {
 public:
  RenderManager(GLFWwindow* window, MeshLoader& loader);

  void beginRender();

  void draw(const Mesh& mesh, const Material& mat);
  void draw(const Model& model);
  void draw(const SceneGraph& graph, MeshLoader& loader);
  void draw(const SceneGraphNode& node, MeshLoader& loader);

  void setViewportSize(int width, int height);

  void setRenderBoundingBoxes(bool shouldRender);

 private:
  // for now, just support one shader program...
  // unique_ptr is just a smart pointer that will automatically call the
  // destructor when out of scope... see
  // https://en.cppreference.com/w/cpp/memory/unique_ptr
  std::unique_ptr<ShaderProgram> mpShaderProgram;
  glm::mat4 mProjection = glm::mat4(1.0f);

  bool mRenderBoundingBoxes = false;
  Model* cubeboi;
};
