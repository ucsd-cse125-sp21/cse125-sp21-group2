#include "MeshLoader.h"

#include <glad/glad.h>

static inline glm::vec3 vec3_cast(aiVector3D& vec) {
  return glm::vec3(vec.x, vec.y, vec.z);
}

unsigned Mesh::vao() const {
  if (mpLoader == nullptr) {
    return 0;
  }
  return mpLoader->getRaw(*this).mVao;
}

unsigned Mesh::vbo() const {
  if (mpLoader == nullptr) {
    return 0;
  }
  return mpLoader->getRaw(*this).mVbo;
}

unsigned Mesh::ibo() const {
  if (mpLoader == nullptr) {
    return 0;
  }
  return mpLoader->getRaw(*this).mIbo;
}

unsigned Mesh::indexCount() const {
  if (mpLoader == nullptr) {
    return 0;
  }
  return mpLoader->getRaw(*this).mIndexCount;
}

MeshLoader::GLMesh::GLMesh(const std::vector<Vertex>& vertices,
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
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               vertices.data(), GL_STATIC_DRAW);

  // configure the vao with vertex attributes
  // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3),
                        nullptr);
  glEnableVertexAttribArray(0);

  // normal
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3),
                        (void*)sizeof(glm::vec3));
  glEnableVertexAttribArray(1);

  glGenBuffers(1, &mIbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(glm::uvec3),
               indices.data(), GL_STATIC_DRAW);

  glBindVertexArray(0);
}

MeshLoader::GLMesh::~GLMesh() {
  glDeleteVertexArrays(1, &mVao);
  glDeleteBuffers(1, &mVbo);
  glDeleteBuffers(1, &mIbo);
}

MeshLoader::GLMesh::GLMesh(GLMesh&& other) noexcept
    : mVao(other.mVao),
      mVbo(other.mVbo),
      mIbo(other.mIbo),
      mIndexCount(other.mIndexCount) {
  // null out opengl resources, so they wont get deleted
  other.mVao = 0;
  other.mVbo = 0;
  other.mIbo = 0;
}
MeshLoader::GLMesh& MeshLoader::GLMesh::operator=(GLMesh&& other) noexcept {
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

Mesh MeshLoader::loadMesh(const std::vector<Vertex>& vertices,
                          const std::vector<glm::uvec3>& indices) {
  mMeshes.emplace_back(vertices, indices);

  // return mesh that points to the new GLMesh in the vector
  return Mesh(mMeshes.size() - 1, this);
}

Mesh MeshLoader::loadMesh(const aiMesh* mesh) {
  std::vector<Vertex> vertices;
  vertices.reserve(mesh->mNumVertices);
  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    vertices.emplace_back(vec3_cast(mesh->mVertices[i]),
                          vec3_cast(mesh->mNormals[i]));
  }

  // get indices out .... must do because they are in ptrs...
  std::vector<glm::uvec3> indices;
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    indices.emplace_back(mesh->mFaces[i].mIndices[0],
                         mesh->mFaces[i].mIndices[1],
                         mesh->mFaces[i].mIndices[2]);
  }

  return loadMesh(vertices, indices);
}

const MeshLoader::GLMesh& MeshLoader::getRaw(const Mesh& mesh) {
  return mMeshes[mesh.mHandle];
}
