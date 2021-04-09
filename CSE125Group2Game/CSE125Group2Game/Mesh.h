﻿#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

#include "SubMesh.h"
#include "Transform.h"
//#include "glad/glad.h"

class Mesh {
 public:
  static Mesh* FromFile(const std::string& path, Transform* transform);
  static Mesh* Cube(Transform* transform);

  Mesh(const std::vector<glm::vec3>& vertices,
       const std::vector<glm::uvec3>& indices, Transform* transform);
  Mesh(const std::string& filePath, Transform* transform);
  /*~Mesh();
  Mesh(const Mesh& other) =
      delete;  // don't allow copying (see rule of three for why)
  Mesh(Mesh&& other) noexcept;                  // move ctor
  Mesh& operator=(const Mesh& other) = delete;  // don't allow copy-assignment
  Mesh& operator=(Mesh&& other) noexcept;       // move-assignment*/

  void draw();

 private:
  // an array of submeshes that makes up this mesh
  std::vector<SubMesh> mSubMeshes;

  Transform*
      mTransform;  // Probably not the best way for now, but good for testing
};
