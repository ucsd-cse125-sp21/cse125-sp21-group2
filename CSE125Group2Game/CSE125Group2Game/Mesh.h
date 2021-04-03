#pragma once

#include "glad/glad.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Mesh
{
public:
  static Mesh Cube();

  Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
  ~Mesh();

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

