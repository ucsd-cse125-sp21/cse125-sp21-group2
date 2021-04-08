#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

#include "SubMesh.h"
#include "glad/glad.h"

class Mesh {
 public:
  static Mesh Cube();
  static Mesh FromFile(const std::string& path);

  Mesh(const std::vector<glm::vec3>& vertices,
       const std::vector<glm::uvec3>& indices);
  Mesh(const std::string& filePath);
  /*~Mesh();
  Mesh(const Mesh& other) =
      delete;  // don't allow copying (see rule of three for why)
  Mesh(Mesh&& other) noexcept;                  // move ctor
  Mesh& operator=(const Mesh& other) = delete;  // don't allow copy-assignment
  Mesh& operator=(Mesh&& other) noexcept;       // move-assignment*/

  void draw();

  void addRotation(glm::vec3 degrees);

 private:
  // an array of submeshes that makes up this mesh
  std::vector<SubMesh> mSubMeshes;

  glm::vec3 mTranslation;
  glm::quat mRotation;
  glm::vec3 mScale;

  glm::mat4 mModel;
};
