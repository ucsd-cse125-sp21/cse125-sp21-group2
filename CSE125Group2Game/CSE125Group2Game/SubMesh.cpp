#include "SubMesh.h"

SubMesh::SubMesh(const std::vector<glm::vec3>& vertices,
                 const std::vector<glm::uvec3>& indices)
    : mIndexCount(indices.size() * 3) {
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

SubMesh::SubMesh(const aiMesh& mesh) {
  std::vector<glm::vec3> vertices;
  vertices.reserve(mesh.mNumVertices);
  for (unsigned int i = 0; i < mesh.mNumVertices; i++) {
    vertices.emplace_back(mesh.mVertices[i].x, mesh.mVertices[i].y,
                          mesh.mVertices[i].z);
  }

  // get indices out .... must do because they are in ptrs...
  std::vector<glm::uvec3> indices;
  for (unsigned int i = 0; i < mesh.mNumFaces; i++) {
    indices.emplace_back(mesh.mFaces[i].mIndices[0], mesh.mFaces[i].mIndices[1],
                         mesh.mFaces[i].mIndices[2]);
  }

  mIndexCount = indices.size() * 3;

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

SubMesh::~SubMesh() {
  glDeleteVertexArrays(1, &mVao);
  glDeleteBuffers(1, &mVbo);
  glDeleteBuffers(1, &mIbo);
}

SubMesh::SubMesh(SubMesh&& other) noexcept
    : mVao(other.mVao),
      mVbo(other.mVbo),
      mIbo(other.mIbo),
      mIndexCount(other.mIndexCount) {
  // null out opengl resources, so they wont get deleted
  other.mVao = 0;
  other.mVbo = 0;
  other.mIbo = 0;
}
SubMesh& SubMesh::operator=(SubMesh&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  // delete our gl resources so we don't leak
  glDeleteVertexArrays(1, &mVao);
  glDeleteBuffers(1, &mVbo);
  glDeleteBuffers(1, &mIbo);

  // copy the handles to 'other's resources
  mVao = other.mVao;
  mVbo = other.mVbo;
  mIbo = other.mIbo;
  mIndexCount = other.mIndexCount;

  // zero out 'other' gl resrouces, we now own them now
  other.mVao = 0;
  other.mVbo = 0;
  other.mIbo = 0;

  return *this;
}

void SubMesh::draw() {
  // bind vertex attributes and index buffer
  glBindVertexArray(mVao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIbo);

  // draw the sub-mesh
  glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, nullptr);
}
