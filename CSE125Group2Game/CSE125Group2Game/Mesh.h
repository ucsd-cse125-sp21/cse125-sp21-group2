#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

#include "glad/glad.h"

class Mesh {
 public:
  static Mesh Cube();

  Mesh(const std::vector<glm::vec3>& vertices,
       const std::vector<glm::uvec3>& indices);
  ~Mesh();
  Mesh(const Mesh& other) =
      delete;  // don't allow copying (see rule of three for why)
  Mesh(Mesh&& other) noexcept;                  // move ctor
  Mesh& operator=(const Mesh& other) = delete;  // don't allow copy-assignment
  Mesh& operator=(Mesh&& other) noexcept;       // move-assignment

  void draw();

  void addRotation(glm::vec3 degrees);

 private:
  GLuint mVao, mVbo, mIbo;
  unsigned int mIndexCount;

  glm::vec3 mTranslation;
  glm::quat mRotation;
  glm::vec3 mScale;

  glm::mat4 mModel;
};
