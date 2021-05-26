#pragma once
#include <assimp/mesh.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <unordered_map>
#include <vector>

struct Vertex {
  glm::vec3 mPosition;
  glm::vec3 mNormal;
  glm::vec2 mUv = glm::vec2(0.0f);       // optional
  glm::vec3 mTangent = glm::vec3(0.0f);  // optional

  Vertex() : mPosition(0.0f), mNormal(0.0f), mUv(0.0f) {}
  Vertex(glm::vec3 position, glm::vec3 normal)
      : mPosition(position), mNormal(normal), mUv(0.0f) {}
  Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv)
      : mPosition(position), mNormal(normal), mUv(uv) {}
  Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv, glm::vec3 tangent)
      : mPosition(position), mNormal(normal), mUv(uv), mTangent(tangent) {}
};

class MeshLoader;

// TODO: This could probably be cleaned up. Something that I really need to do
// that will be important is ensuring the public API can be somewhat stable, So
// the gameplay or others don't need to change things around as I work on
// rendering...

// TODO: move to be internal class of MeshLoader?
class Mesh {
 public:
  Mesh() : mHandle(-1), mpLoader(nullptr) {}
  Mesh(const Mesh& other) = default;
  Mesh(Mesh&& other) noexcept = default;
  Mesh& operator=(const Mesh& other) = default;
  Mesh& operator=(Mesh&& other) noexcept = default;

  unsigned int vao() const;
  unsigned int vbo() const;
  unsigned int ibo() const;
  unsigned int indexCount() const;
  bool isValid() const { return mHandle != -1; }

 private:
  Mesh(int handle, MeshLoader* loader) : mHandle(handle), mpLoader(loader) {}
  int mHandle;

  // kinda gross, but means we don't need to pass loader everywhere
  MeshLoader* mpLoader;
  friend MeshLoader;
};

class MeshLoader {
 public:
  class GLMesh {
   public:
    unsigned int mVao, mVbo, mIbo, mIndexCount = 0;

    GLMesh(const std::vector<Vertex>& vertices,
           const std::vector<glm::uvec3>& indices);
    ~GLMesh();
    GLMesh(const GLMesh& other) = delete;
    GLMesh(GLMesh&& other) noexcept;
    GLMesh& operator=(const GLMesh& other) = delete;
    GLMesh& operator=(GLMesh&& other) noexcept;

   private:
    void init(const std::vector<Vertex>& vertices,
              const std::vector<glm::uvec3>& indices);
  };

  Mesh loadMesh(const std::vector<Vertex>& vertices,
                const std::vector<glm::uvec3>& indices);
  Mesh loadMesh(const aiMesh* mesh);

  Mesh Square();
  Mesh Cube();

  const GLMesh& getRaw(const Mesh& mesh);

 private:
  // for simplicity, lets not do instancing, and lets assume objects live
  // forever...
  std::vector<GLMesh> mMeshes;
  Mesh mSquare;
  Mesh mCube;
};
