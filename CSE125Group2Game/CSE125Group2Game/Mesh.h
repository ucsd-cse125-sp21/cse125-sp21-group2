#pragma once

#include "glad/glad.h"
#include "Transform.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Mesh
{
public:
  static Mesh Cube(Transform* transform);

  Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, Transform* transform);
  ~Mesh();

  void draw();


private:
  GLuint mVao, mVbo, mIbo;
  unsigned int mIndexCount;

  Transform* mTransform; // Probably not the best way for now, but good for testing
};

