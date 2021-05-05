#include "RenderManager.h"

#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "Camera.h"
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
RenderManager::RenderManager(GLFWwindow* window, MeshLoader& loader,
                             TextureLoader& tloader, Camera* camera)
    : mpCamera(camera) {
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

  mpColorProgram =
      std::make_unique<ShaderProgram>("color_vert.glsl", "color_frag.glsl");
  mpNormalProgram =
      std::make_unique<ShaderProgram>("normal_vert.glsl", "normal_frag.glsl");
  mpTextureProgram =
      std::make_unique<ShaderProgram>("texture_vert.glsl", "texture_frag.glsl");
  mpRainbowProgram =
      std::make_unique<ShaderProgram>("rainbow_vert.glsl", "rainbow_frag.glsl");

  mProjection =
      glm::perspective(glm::radians(FOVY), static_cast<float>(width) / height,
                       NEAR_CLIP, FAR_CLIP);

  mpColorProgram->use();
  mpTextureProgram->use();
  mTexLoader = &tloader;

  cubeboi = Model::Cube(nullptr, loader);
}

/**
 * Must be called before a series of render calls, to clear render state, etc.
 */
void RenderManager::beginRender() {
  glClearColor(0.2f, 0.4f, 0.4f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (mUseNormalShading) {
    mpNormalProgram->use();
  } else {
    mpTextureProgram->use();
    // mpColorProgram->use();
  }

  glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(mProjection));
}

void RenderManager::draw(const Mesh& mesh, const Material& mat,
                         const glm::mat4& model, const glm::mat4& view) {
  glBindTexture(GL_TEXTURE_2D, 1);
  // set material colors...
  // in this case, we should use the texture shader
  if (mat.diffuseMap.isValid()) {
    mpTextureProgram->use();
    mTexLoader->use(mat.diffuseMap);
  } else {
    mpColorProgram->use();
    // glUniform1f(7, currentTime);
    glUniform3fv(3, 1, glm::value_ptr(mat.mAmbient));
    glUniform3fv(4, 1, glm::value_ptr(mat.mDiffuse));
    glUniform3fv(5, 1, glm::value_ptr(mat.mSpecular));
    glUniform1fv(6, 1, &mat.mShininess);
  }
  glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(mProjection));
  glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(model));

  // render the mesh
  glBindVertexArray(mesh.vao());
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo());
  glDrawElements(GL_TRIANGLES, mesh.indexCount(), GL_UNSIGNED_INT, nullptr);
}

/*void RenderManager::draw(const Model& model, const glm::mat4& view) {
  draw(model, glm::mat4(1.0f), view);
}*/

void RenderManager::draw(const Model& model, const glm::mat4& transform,
                         const glm::mat4& view) {
  for (int i = 0; i < model.meshes().size(); i++) {
    draw(model.meshes()[i], model.materials()[i], transform, view);
  }
}

void RenderManager::draw(const SceneGraph& graph, MeshLoader& loader) {
  SceneGraphNode* root = graph.getRoot();
  auto viewOption = graph.getViewMatrix();

  // what should default behavior be? i.e. if there is no camera, what do we do?
  // id matrix? idk
  if (!viewOption.has_value()) {
    // for now, if we don't ahve camera, just crash to make it easy to notice
    // LOL
    CRASH(
        "There isn't a camera! This probably isn't fatal, but Evan wanted to "
        "make it easy to debug for now");
  }

  auto view = viewOption.value();
  draw(*root, loader, glm::mat4(1), view);
}

// TODO: fix scene graph, then this will need to change.
void RenderManager::draw(const SceneGraphNode& node, MeshLoader& loader,
                         const glm::mat4& prev, const glm::mat4& view) {
  glm::mat4 currTransform = prev * node.getObject()->getTransform()->getModel();
  // TODO: PLEASE REFACTOR :((
  if (mRenderBoundingBoxes) {
    auto bb = node.getObject()->getTransform()->getBBox();

    glm::mat4 model =
        currTransform *
        glm::scale(glm::mat4(1), 2.0f * glm::vec3(bb.x, bb.y, bb.z));

    glm::vec4 god[8];
    // this is gross
    god[0] = model * glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f);
    god[1] = model * glm::vec4(0.5f, -0.5f, 0.5f, 1.0f);
    god[2] = model * glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f);
    god[3] = model * glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    god[4] = model * glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f);
    god[5] = model * glm::vec4(0.5f, 0.5f, -0.5f, 1.0f);
    god[6] = model * glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f);
    god[7] = model * glm::vec4(0.5f, -0.5f, -0.5f, 1.0f);

    glm::vec3 max = god[0];
    glm::vec3 min = god[0];
    for (int i = 0; i < 8; i++) {
      if (god[i].x > max.x) {
        max.x = god[i].x;
      }
      if (god[i].y > max.y) {
        max.y = god[i].y;
      }
      if (god[i].z > max.z) {
        max.z = god[i].z;
      }
      if (god[i].x < min.x) {
        min.x = god[i].x;
      }
      if (god[i].y < min.y) {
        min.y = god[i].y;
      }
      if (god[i].z < min.z) {
        min.z = god[i].z;
      }
    }

    glm::vec3 scale = max - min;
    glm::vec3 trans = glm::vec3(model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    glm::mat4 mememodel =
        glm::scale(glm::translate(glm::mat4(1.0f), trans), scale);

    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mememodel));

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    draw(cubeboi->meshes()[0],
         Material(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f),
                  0),
         mememodel, view);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  auto model = node.getModel();
  if (model) {
    draw(*model, currTransform, view);
  }

  for (auto child : node.getChildren()) {
    draw(*child, loader, currTransform, view);
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

void RenderManager::setRenderBoundingBoxes(bool shouldRender) {
  mRenderBoundingBoxes = shouldRender;
}

void RenderManager::setNormalShading(bool useNormalShading) {
  mUseNormalShading = useNormalShading;
}
