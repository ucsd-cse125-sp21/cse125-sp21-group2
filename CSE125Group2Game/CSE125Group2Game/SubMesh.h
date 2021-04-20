#pragma once
#include <assimp/mesh.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <vector>

/**
 * An OpenGL smart-pointer representing a SubMesh, where a SubMesh is an
 * individual part of a mesh with a particular material.
 */
class SubMesh {
 public:
  SubMesh() : mVao(0), mVbo(0), mIbo(0), mIndexCount(0){};
  SubMesh(const std::vector<glm::vec3>& vertices,
          const std::vector<glm::uvec3>& indices);
  SubMesh(const aiMesh& mesh);
  ~SubMesh();
  SubMesh(const SubMesh& other) = delete;
  SubMesh(SubMesh&& other) noexcept;
  SubMesh& operator=(const SubMesh& other) = delete;
  SubMesh& operator=(SubMesh&& other) noexcept;

  void draw();

 private:
  GLuint mVao, mVbo, mIbo;
  unsigned int mIndexCount;
};
