#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <memory>

#include "Camera.h"
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
  RenderManager(GLFWwindow* window, MeshLoader& loader, TextureLoader& tloader);

  void beginRender();
  void draw(const Mesh& mesh, const Material& mat, const glm::mat4& model);
  void draw(const Model& model);
  void draw(const SceneGraph& graph, MeshLoader& loader);
  void draw(const SceneGraphNode& node, MeshLoader& loader);

  void setViewportSize(int width, int height);

  void setNormalShading(bool useNormalShading);

  void setRenderBoundingBoxes(bool shouldRender);

  Camera* mCamera;

 private:
  TextureLoader* mTexLoader;
  MeshLoader* mModelLoader;
  // List of supported shaders...
  // for now, just support one shader program...
  // unique_ptr is just a smart pointer that will automatically call the
  // destructor when out of scope... see
  // https://en.cppreference.com/w/cpp/memory/unique_ptr
  std::unique_ptr<ShaderProgram> mpColorProgram;
  std::unique_ptr<ShaderProgram> mpNormalProgram;
  std::unique_ptr<ShaderProgram> mpTextureProgram;

  bool mUseNormalShading = false;

  glm::mat4 mProjection = glm::mat4(1.0f);

  bool mRenderBoundingBoxes = false;
  Model* cubeboi;
};
