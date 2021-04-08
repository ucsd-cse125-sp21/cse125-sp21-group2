#include "Mesh.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

Mesh Mesh::Cube() {
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

  return Mesh(vertices, indices);
}

Mesh::Mesh(const std::vector<glm::vec3>& vertices,
           const std::vector<glm::uvec3>& indices)
    : mVao(0),
      mVbo(0),
      mIbo(0),
      mIndexCount(indices.size() * 3),
      mTranslation(0.0f),
      mRotation(glm::vec3(0, 0, 0)),
      mScale(1.0f),
      mModel(1.0f) {
  // create vertex array object
  glGenVertexArrays(1, &mVao);

  // configure the vertex array object
  glBindVertexArray(mVao);

  // create vbo for triangle vertices
  glGenBuffers(1, &mVbo);

  // bind it as array buffer
  glBindBuffer(GL_ARRAY_BUFFER, mVbo);

  // copy vertex data to GPU buffer
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
               vertices.data(), GL_STATIC_DRAW);

  // configure the vao with vertex attributes
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  glGenBuffers(1, &mIbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(glm::uvec3),
               indices.data(), GL_STATIC_DRAW);

  glBindVertexArray(0);
}

Mesh::Mesh(Mesh&& other) noexcept
    : mVao(other.mVao),
      mVbo(other.mVbo),
      mIbo(other.mIbo),
      mIndexCount(other.mIndexCount),
      mModel(other.mModel),
      mTranslation(other.mTranslation),
      mRotation(other.mRotation),
      mScale(other.mScale) {
  // null out opengl resources, so they wont get deleted
  other.mVao = 0;
  other.mVbo = 0;
  other.mIbo = 0;
}

Mesh::~Mesh() {
  glDeleteVertexArrays(1, &mVao);
  glDeleteBuffers(1, &mVbo);
  glDeleteBuffers(1, &mIbo);
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  // delete our gl resources so we don't leak
  glDeleteVertexArrays(1, &mVao);
  glDeleteBuffers(1, &mVbo);
  glDeleteBuffers(1, &mIbo);

  mVao = other.mVao;
  mVbo = other.mVbo;
  mIbo = other.mIbo;
  mIndexCount = other.mIndexCount;

  mModel = other.mModel;
  mTranslation = other.mTranslation;
  mRotation = other.mRotation;
  mScale = other.mScale;

  // zero out 'other' gl resrouces, we now own the
  other.mVao = 0;
  other.mVbo = 0;
  other.mIbo = 0;
}

void Mesh::draw() {
  glBindVertexArray(mVao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIbo);

  glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mModel));

  glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, nullptr);
}

void Mesh::addRotation(glm::vec3 degrees) {
  mRotation = glm::quat(glm::radians(degrees)) * mRotation;

  mModel = glm::translate(glm::mat4(1.0f), mTranslation) *
           glm::toMat4(mRotation) * glm::scale(glm::mat4(1.0f), mScale);
}
