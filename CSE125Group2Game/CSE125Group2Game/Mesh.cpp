#include "Mesh.h"

#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

Mesh Mesh::Cube()
{
  std::vector<float> vertices {
    // front
    -0.5, -0.5,  0.5,
     0.5, -0.5,  0.5,
     0.5,  0.5,  0.5,
    -0.5,  0.5,  0.5,

    // back
    -0.5, -0.5, -0.5,
     0.5, -0.5, -0.5,
     0.5,  0.5, -0.5,
    -0.5,  0.5, -0.5
  };

  std::vector<float> colors {
    // front colors
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0,
    1.0, 1.0, 1.0,
    // back colors
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0,
    1.0, 1.0, 1.0
  };

  std::vector<unsigned int> indices {
    // front
    0, 1, 2,
    2, 3, 0,
    // right
    1, 5, 6,
    6, 2, 1,
    // back
    7, 6, 5,
    5, 4, 7,
    // left
    4, 0, 3,
    3, 7, 4,
    // bottom
    4, 5, 1,
    1, 0, 4,
    // top
    3, 2, 6,
    6, 7, 3
  };

  return Mesh(vertices, indices);
}

Mesh::Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) : 
  mVao(0), mVbo(0), mIbo(0), mIndexCount(indices.size()), mTranslation(0.0f), mRotation(glm::vec3(0, 0, 0)), mScale(1.0f), mModel(1.0f) {

  // create vertex array object
  glGenVertexArrays(1, &mVao);

  // configure the vertex array object
  glBindVertexArray(mVao);

  // create vbo for triangle vertices
  glGenBuffers(1, &mVbo);

  // bind it as array buffer
  glBindBuffer(GL_ARRAY_BUFFER, mVbo);

  // copy vertex data to GPU buffer
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

  // configure the vao with vertex attributes
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  glGenBuffers(1, &mIbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

  glBindVertexArray(0);
}

Mesh::~Mesh()
{
  glDeleteVertexArrays(1, &mVao);
  glDeleteBuffers(1, &mVbo);
  glDeleteBuffers(1, &mIbo);
}

void Mesh::draw() {
  glBindVertexArray(mVao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIbo);

  glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mModel));

  glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, nullptr);
}

void Mesh::addRotation(glm::vec3 degrees) {
  mRotation = glm::quat(glm::radians(degrees)) * mRotation;

  mModel = glm::translate(glm::mat4(1.0f), mTranslation) * glm::toMat4(mRotation) * glm::scale(glm::mat4(1.0f), mScale);
}
