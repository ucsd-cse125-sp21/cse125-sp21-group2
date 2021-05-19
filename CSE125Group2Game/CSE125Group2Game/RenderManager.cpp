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

#define FOVY 100.0f
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
  // glEnable(GL_CULL_FACE);
  // glCullFace(GL_BACK);

  glEnable(GL_DEPTH_TEST);

  // tell opengl to multisample...
  glEnable(GL_MULTISAMPLE);

  // enable blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
  mpBumpProgram =
      std::make_unique<ShaderProgram>("bump_vert.glsl", "bump_frag.glsl");
  mpParticleProgram = std::make_unique<ShaderProgram>("particle_vert.glsl",
                                                      "particle_frag.glsl");
  mpSkyboxProgram =
      std::make_unique<ShaderProgram>("skybox_vert.glsl", "skybox_frag.glsl");

  mProjection =
      glm::perspective(glm::radians(FOVY), static_cast<float>(width) / height,
                       NEAR_CLIP, FAR_CLIP);

  mpColorProgram->use();
  mpTextureProgram->use();
  mTexLoader = &tloader;

  cubeboi = Model::Cube(loader);
}

/**
 * Must be called before a series of render calls, to clear render state, etc.
 */
void RenderManager::beginRender() {
  currentTime = glfwGetTime();

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
                         const glm::mat4& model, const glm::mat4& view,
                         const glm::vec3& viewPos) {
  // set material colors...
  // in this case, we should use the texture shader
  // TODO: refactor this mess
  if (mat.normalMap.isValid()) {
    mpBumpProgram->use();
    glUniform1i(glGetUniformLocation(mpBumpProgram->getID(), "diffuseMap"), 0);
    glActiveTexture(GL_TEXTURE0);
    mTexLoader->use(mat.diffuseMap);
    // TODO: this stuff can be refactored out side of render loop probably
    glUniform1i(glGetUniformLocation(mpBumpProgram->getID(), "normalMap"), 1);
    glActiveTexture(GL_TEXTURE1);
    mTexLoader->use(mat.normalMap);

    glUniform3fv(3, 1, glm::value_ptr(mat.mAmbient));
    glUniform3fv(4, 1, glm::value_ptr(mat.mDiffuse));
    glUniform3fv(5, 1, glm::value_ptr(mat.mSpecular));
    glUniform1fv(6, 1, &mat.mShininess);
    glUniform3fv(8, 1, glm::value_ptr(viewPos));
  } else if (mat.diffuseMap.isValid()) {
    mpTextureProgram->use();
    glUniform1i(glGetUniformLocation(mpTextureProgram->getID(), "diffuseMap"),
                0);
    glActiveTexture(GL_TEXTURE0);
    mTexLoader->use(mat.diffuseMap);

    glUniform3fv(3, 1, glm::value_ptr(mat.mAmbient));
    glUniform3fv(4, 1, glm::value_ptr(mat.mDiffuse));
    glUniform3fv(5, 1, glm::value_ptr(mat.mSpecular));
    glUniform1fv(6, 1, &mat.mShininess);
    glUniform3fv(8, 1, glm::value_ptr(viewPos));
  } else if (mat.isRainbow) {
    mpRainbowProgram->use();
    glUniform1f(7, currentTime);
  } else {
    mpColorProgram->use();
    glUniform3fv(3, 1, glm::value_ptr(mat.mAmbient));
    glUniform3fv(4, 1, glm::value_ptr(mat.mDiffuse));
    glUniform3fv(5, 1, glm::value_ptr(mat.mSpecular));
    glUniform1fv(6, 1, &mat.mShininess);
    glUniform3fv(8, 1, glm::value_ptr(viewPos));
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
                         const glm::mat4& view, const glm::vec3& viewPos) {
  for (int i = 0; i < model.meshes().size(); i++) {
    draw(model.meshes()[i], model.materials()[i], transform, view, viewPos);
  }
}

void RenderManager::draw(const SceneGraph& graph, MeshLoader& loader) {
  SceneGraphNode* root = graph.getRoot();
  auto viewOption = graph.getCameraMatrix();

  // what should default behavior be? i.e. if there is no camera, what do we do?
  // id matrix? idk
  if (!viewOption.has_value()) {
    // TODO?
  }

  // get the view matrix from the camera
  auto cameraMatrix = viewOption.value_or(glm::mat4(1.0f));
  glm::vec3 viewPos = cameraMatrix * glm::vec4(0, 0, 0, 1);
  auto view = glm::inverse(cameraMatrix);

  // draw the skybox if it exists
  if (graph.getSkybox()) {
    // TODO: gross, but necessary or will cull skybox
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    mpSkyboxProgram->use();

    // don't include translate in skybox view
    glUniformMatrix4fv(1, 1, GL_FALSE,
                       glm::value_ptr(glm::mat4(glm::mat3(view))));
    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(mProjection));

    // render the skybox
    glBindVertexArray(graph.getSkybox()->mpCube->meshes()[0].vao());
    mTexLoader->use(graph.getSkybox()->mCubeMap);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                 graph.getSkybox()->mpCube->meshes()[0].ibo());
    glDrawElements(GL_TRIANGLES,
                   graph.getSkybox()->mpCube->meshes()[0].indexCount(),
                   GL_UNSIGNED_INT, nullptr);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
  }

  // recursivly render the scene
  draw(*root, loader, glm::mat4(1), view, viewPos);
  // draw all particles next
  drawParticles(*root, glm::mat4(1), view, viewPos);
}

// TODO: fix scene graph, then this will need to change.
void RenderManager::draw(const SceneGraphNode& node, MeshLoader& loader,
                         const glm::mat4& prev, const glm::mat4& view,
                         const glm::vec3& viewPos) {
  // Added by network folks :D
  if (!node.getObject()->mShouldRender) {
    return;
  }

  glm::mat4 currTransform = prev * node.getObject()->getTransform()->getModel();
  if (mRenderBoundingBoxes) {
    drawBoundingBox(node, currTransform, view, viewPos);
  }

  auto model = node.getModel();

  if (model) {
    draw(*model, currTransform, view, viewPos);
  }

  for (auto child : node.getChildren()) {
    draw(*child, loader, currTransform, view, viewPos);
  }
}

void RenderManager::drawParticles(const SceneGraphNode& node,
                                  const glm::mat4& prev, const glm::mat4& view,
                                  const glm::vec3& viewPos) {
  glm::mat4 currTransform = prev * node.getObject()->getTransform()->getModel();
  auto model = node.getModel();
  if (node.emitter) {
    draw(*node.emitter, currTransform, view, viewPos);
  }

  for (auto child : node.getChildren()) {
    drawParticles(*child, currTransform, view, viewPos);
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

// TODO: remove
void RenderManager::draw(ParticleEmitter& emitter, const glm::mat4& model,
                         const glm::mat4& view, const glm::vec3& viewPos) {
  mpParticleProgram->use();
  glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(mProjection));
  glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(model));

  glUniform1i(glGetUniformLocation(mpParticleProgram->getID(), "tex"), 0);
  glActiveTexture(GL_TEXTURE0);

  mTexLoader->use(emitter.mParticleTexture);

  // set the uniforms
  glUniform1f(3, emitter.mParticleSize);
  glUniform1i(4, emitter.mAnealSize);

  // TODO: this is WRONG, with multiple particle effects, we need to layer them
  // correctly sort particles based on camera position
  std::sort(
      emitter.mParticles.begin(), emitter.mParticles.end(),
      [&](const Particle& a, const Particle& b) {
        const float a_dist =
            glm::length(viewPos - glm::vec3(model * glm::vec4(a.position, 1)));
        const float b_dist =
            glm::length(viewPos - glm::vec3(model * glm::vec4(b.position, 1)));
        return a_dist > b_dist;
      });

  // TODO: for now, just assume shader set, uniforms are set, etc.
  glBindVertexArray(emitter.mVao);

  // update the particle buffer
  glBindBuffer(GL_ARRAY_BUFFER, emitter.mParticleVBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, emitter.mNumParticles * sizeof(Particle),
                  emitter.mParticles.data());

  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, emitter.mNumParticles);
}

void RenderManager::drawBoundingBox(const SceneGraphNode& node,
                                    const glm::mat4& currTransform,
                                    const glm::mat4& view,
                                    const glm::vec3& viewPos) {
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
       mememodel, view, viewPos);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
