#include "RenderManager.h"

#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "Model.h"
#include "Utils.h"

#define FOVY 90.0f
#define NEAR_CLIP 0.1f
#define FAR_CLIP 100.0f

/**
 * Initializes the RenderManager using the passed in window as the render
 * target.
 *
 * This method should only be called once.
 *
 * @param window The window to use as a render target.
 * @returns True if successful, false otherwise...
 */
RenderManager::RenderManager(GLFWwindow* window) {
  // tell opengl to use the current window context
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    CRASH("Failed to load glad!");
  }

  // ensure we cull the back face to prevent wacky artifacts
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnable(GL_DEPTH_TEST);

  // tell opengl to multisample...
  glEnable(GL_MULTISAMPLE);

  // tell opengl our screen coordinate ranges
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

  mpShaderProgram =
      std::make_unique<ShaderProgram>("vertex.glsl", "fragment.glsl");

  mProjection =
      glm::perspective(glm::radians(FOVY), static_cast<float>(width) / height,
                       NEAR_CLIP, FAR_CLIP);
}

/**
 * Must be called before a series of render calls, to clear render state, etc.
 */
void RenderManager::beginRender() {
  glClearColor(0.2f, 0.4f, 0.4f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  mpShaderProgram->use();

  glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(mProjection));
}

void RenderManager::draw(const Mesh& mesh, const Material& mat) {
  // set material colors...
  glUniform3fv(3, 1, glm::value_ptr(mat.ambient));
  glUniform3fv(4, 1, glm::value_ptr(mat.diffuse));
  glUniform3fv(5, 1, glm::value_ptr(mat.specular));
  glUniform1fv(6, 1, &mat.shininess);

  // render the mesh
  glBindVertexArray(mesh.vao());
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo());
  glDrawElements(GL_TRIANGLES, mesh.indexCount(), GL_UNSIGNED_INT, nullptr);
}

void RenderManager::draw(const Model& model) {
  glUniformMatrix4fv(0, 1, GL_FALSE,
                     glm::value_ptr(model.transformConst().getModel()));
  for (int i = 0; i < model.meshes().size(); i++) {
    draw(model.meshes()[i], model.materials()[i]);
  }
}

void RenderManager::draw(const SceneGraph& graph) {
  auto root = graph.getRoot();
  draw(*root);
}

// TODO: fix scene graph, then this will need to change.
void RenderManager::draw(const SceneGraphNode& node) {
  auto model = node.getModel();
  if (model) {
    draw(*model);
  }

  for (auto child : node.getChildren()) {
    draw(*child);
  }
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
  mProjection =
      glm::perspective(glm::radians(FOVY), static_cast<float>(width) / height,
                       NEAR_CLIP, FAR_CLIP);
}
