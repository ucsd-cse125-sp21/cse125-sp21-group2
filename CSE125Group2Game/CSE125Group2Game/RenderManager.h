#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <memory>

#include "Camera.h"
#include "Font.h"
#include "Image.h"
#include "MeshLoader.h"
#include "Model.h"
#include "ParticleEmitter.h"
#include "Rect.h"
#include "SceneGraph.h"
#include "ShaderProgram.h"
#include "Skybox.h"

// singleton
/* Responsibilities:
 * - Initializing opengl
 * - Shader compilation/management
 */
class RenderManager {
 public:
  RenderManager(GLFWwindow* window, MeshLoader& loader, TextureLoader& tloader,
                Camera* camera);

  void beginRender();
  void draw(const Mesh& mesh, const Material& mat, const glm::mat4& model,
            const glm::mat4& view, const glm::vec3& viewPos,
            const bool rainbow);
  void draw(const Model& model, const glm::mat4& view);
  void draw(const Model& model, const glm::mat4& transform,
            const glm::mat4& view, const glm::vec3& viewPos);
  void draw(const SceneGraph& graph, MeshLoader& loader);
  void draw(const SceneGraphNode& node, MeshLoader& loader,
            const glm::mat4& prev, const glm::mat4& view,
            const glm::vec3& viewPos);
  void drawParticles(const SceneGraphNode& node, const glm::mat4& prev,
                     const glm::mat4& view, const glm::vec3& viewPos);
  void drawHealthBars(const SceneGraphNode& node, const glm::mat4& prev,
                      const glm::mat4& view, const glm::vec3& viewPos);

  // refactor this
  void draw(ParticleEmitter& emitter, const glm::mat4& model,
            const glm::mat4& view, const glm::vec3& viewPos);

  // TODO: refactor into UI class
  void drawText(const std::string text, float x, float y, float scale,
                const glm::vec3& color, const Font& ui);
  void drawRect(const Rect& rect);
  void drawImage(const Image& image);
  void drawHealthBar(const glm::mat4& prev, const glm::mat4& view,
                     const glm::vec3& viewPos, float health);

  void setViewportSize(int width, int height);

  void setNormalShading(bool useNormalShading);

  void setRenderBoundingBoxes(bool shouldRender);

  void updateTime(float delta) { currentTime += delta; };

  // TODO: make render manager own this eventually, or maybe scene graph idk
  Camera* mpCamera;

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
  std::unique_ptr<ShaderProgram> mpRainbowProgram;
  std::unique_ptr<ShaderProgram> mpBumpProgram;
  std::unique_ptr<ShaderProgram> mpParticleProgram;
  std::unique_ptr<ShaderProgram> mpSkyboxProgram;
  std::unique_ptr<ShaderProgram> mpFontProgram;
  std::unique_ptr<ShaderProgram> mpBillboardProgram;
  std::unique_ptr<ShaderProgram> mpRectProgram;
  std::unique_ptr<ShaderProgram> mpImageProgram;

  bool mUseNormalShading = false;

  glm::mat4 mProjection = glm::mat4(1.0f);
  glm::mat4 mUIProjection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);

  bool mRenderBoundingBoxes = false;
  Model* cubeboi;

  float currentTime = 0.0f;

  // helpers
  void drawBoundingBox(const SceneGraphNode& node,
                       const glm::mat4& currTransform, const glm::mat4& view,
                       const glm::vec3& viewPos);
};
