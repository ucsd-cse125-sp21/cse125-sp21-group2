#include "Mesh.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>

Mesh* Mesh::Cube(Transform* transform) {
  std::vector<glm::vec3> vertices{
      // front
      glm::vec3(-0.5, -0.5, 0.5), glm::vec3(0.5, -0.5, 0.5),
      glm::vec3(0.5, 0.5, 0.5), glm::vec3(-0.5, 0.5, 0.5),

      // back
      glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0.5, -0.5, -0.5),
      glm::vec3(0.5, 0.5, -0.5), glm::vec3(-0.5, 0.5, -0.5)};

  std::vector<glm::vec3> colors{
      // front colors
      glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0),
      glm::vec3(0.0, 0.0, 1.0), glm::vec3(1.0, 1.0, 1.0),
      // back colors
      glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0),
      glm::vec3(0.0, 0.0, 1.0), glm::vec3(1.0, 1.0, 1.0)};

  std::vector<glm::uvec3> indices{// front
                                  glm::uvec3(0, 1, 2), glm::uvec3(2, 3, 0),
                                  // right
                                  glm::uvec3(1, 5, 6), glm::uvec3(6, 2, 1),
                                  // back
                                  glm::uvec3(7, 6, 5), glm::uvec3(5, 4, 7),
                                  // left
                                  glm::uvec3(4, 0, 3), glm::uvec3(3, 7, 4),
                                  // bottom
                                  glm::uvec3(4, 5, 1), glm::uvec3(1, 0, 4),
                                  // top
                                  glm::uvec3(3, 2, 6), glm::uvec3(6, 7, 3)};

  return new Mesh(vertices, indices, transform);
}

Mesh::Mesh(const std::vector<glm::vec3>& vertices,
           const std::vector<glm::uvec3>& indices, Transform* transform)
    : mTransform(transform), mSubMeshes(0) {
  mSubMeshes.emplace_back(vertices, indices);
}

/**
 * Loads a mesh from a model file using Assimp.
 *
 * TODO: Does not currently support many object/mesh types. Need to add
 * submeshes, materials, etc to work properly.
 * TODO: are exceptions the right way to go here? Probably
 *
 * @throws std::exception Exception thrown when there is an error loading the
 * file.
 */
Mesh::Mesh(const std::string& filePath, Transform* transform)
    : mTransform(transform) {
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(
      filePath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cout << "Error Assimp loading..." << std::endl;
    throw std::exception("failed to load file...");
  }

  mSubMeshes.reserve(scene->mNumMeshes);
  for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
    mSubMeshes.emplace_back(*scene->mMeshes[i]);
  }
}

void Mesh::draw() {
  glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mTransform->getModel()));
  for (SubMesh& subMesh : mSubMeshes) {
    subMesh.draw();
  }
}
