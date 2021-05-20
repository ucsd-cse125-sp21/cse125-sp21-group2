#include "Model.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>

#include "Utils.h"

Model* Model::Cube(MeshLoader& loader) {
  std::vector<glm::vec3> positions{
      // front
      glm::vec3(-0.5, -0.5, 0.5),
      glm::vec3(0.5, -0.5, 0.5),
      glm::vec3(-0.5, 0.5, 0.5),
      glm::vec3(0.5, 0.5, 0.5),

      // back
      glm::vec3(-0.5, 0.5, -0.5),
      glm::vec3(0.5, 0.5, -0.5),
      glm::vec3(-0.5, -0.5, -0.5),
      glm::vec3(0.5, -0.5, -0.5),
  };

  std::vector<glm::vec3> normals{
      glm::vec3(0.0, 0.0, 1.0),  glm::vec3(0.0, 1.0, 0.0),
      glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0),
      glm::vec3(1.0, 0.0, 0.0),  glm::vec3(-1.0, 0.0, 0.0),
  };

  std::vector<Vertex> vertices{
      // face 0
      Vertex(positions[0], normals[0]),
      Vertex(positions[1], normals[0]),
      Vertex(positions[2], normals[0]),
      Vertex(positions[2], normals[0]),
      Vertex(positions[1], normals[0]),
      Vertex(positions[3], normals[0]),

      // face 1
      Vertex(positions[2], normals[1]),
      Vertex(positions[3], normals[1]),
      Vertex(positions[4], normals[1]),
      Vertex(positions[4], normals[1]),
      Vertex(positions[3], normals[1]),
      Vertex(positions[5], normals[1]),

      // face 2
      Vertex(positions[4], normals[2]),
      Vertex(positions[5], normals[2]),
      Vertex(positions[6], normals[2]),
      Vertex(positions[6], normals[2]),
      Vertex(positions[5], normals[2]),
      Vertex(positions[7], normals[2]),

      // face 3
      Vertex(positions[6], normals[3]),
      Vertex(positions[7], normals[3]),
      Vertex(positions[0], normals[3]),
      Vertex(positions[0], normals[3]),
      Vertex(positions[7], normals[3]),
      Vertex(positions[1], normals[3]),

      // face 4
      Vertex(positions[1], normals[4]),
      Vertex(positions[7], normals[4]),
      Vertex(positions[3], normals[4]),
      Vertex(positions[3], normals[4]),
      Vertex(positions[7], normals[4]),
      Vertex(positions[5], normals[4]),

      // face 5
      Vertex(positions[6], normals[5]),
      Vertex(positions[0], normals[5]),
      Vertex(positions[4], normals[5]),
      Vertex(positions[4], normals[5]),
      Vertex(positions[0], normals[5]),
      Vertex(positions[2], normals[5]),
  };

  std::vector<glm::vec3> colors{
      // front colors
      glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0),
      glm::vec3(0.0, 0.0, 1.0), glm::vec3(1.0, 1.0, 1.0),
      // back colors
      glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0),
      glm::vec3(0.0, 0.0, 1.0), glm::vec3(1.0, 1.0, 1.0)};

  std::vector<glm::uvec3> indices{
      // front
      glm::uvec3(0, 1, 2),
      glm::uvec3(3, 4, 5),
      // right
      glm::uvec3(6, 7, 8),
      glm::uvec3(9, 10, 11),
      // back
      glm::uvec3(12, 13, 14),
      glm::uvec3(15, 16, 17),
      // left
      glm::uvec3(18, 19, 20),
      glm::uvec3(21, 22, 23),
      // bottom
      glm::uvec3(24, 25, 26),
      glm::uvec3(27, 28, 29),
      // top
      glm::uvec3(30, 31, 32),
      glm::uvec3(33, 34, 35),
  };

  return new Model(vertices, indices, loader);
}

Model::Model(const std::vector<Vertex>& vertices,
             const std::vector<glm::uvec3>& indices, MeshLoader& loader)
    : mMeshes(0) {
  mMeshes.push_back(loader.loadMesh(vertices, indices));
  mMaterials.push_back(Material());
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
Model::Model(const std::string& filePath, MeshLoader& loader,
             TextureLoader& tloader) {
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(
      filePath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
                    aiProcess_CalcTangentSpace);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    CRASH("Failed to load model " + filePath + "!");
  }

  mMeshes.reserve(scene->mNumMeshes);
  mMaterials.reserve(scene->mNumMeshes);
  for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
    aiMesh* mesh = scene->mMeshes[i];
    mMeshes.push_back(loader.loadMesh(mesh));

    // populate our material structs
    // TODO: right now we just copy for each mesh, even if they use
    //    the same material.
    aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];
    mMaterials.push_back(Material(aiMat, tloader));
  }
}
