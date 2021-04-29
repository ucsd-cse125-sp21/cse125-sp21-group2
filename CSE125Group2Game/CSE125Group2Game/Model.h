#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

#include "Material.h"
#include "MeshLoader.h"
#include "Transform.h"

class Model {
 public:
  static Model* Cube(Transform* transform, MeshLoader& loader);

  Model(const std::vector<Vertex>& vertices,
        const std::vector<glm::uvec3>& indices, Transform* transform,
        MeshLoader& loader);
  Model(const std::string& filePath, Transform* transform, MeshLoader& loader,
        TextureLoader& tloader);

  // TODO: may want to refactor this for better encapsulation...
  Transform& transform() { return *mTransform; }
  Transform transformConst() const { return *mTransform; }
  const std::vector<Mesh>& meshes() const { return mMeshes; }
  const std::vector<Material>& materials() const { return mMaterials; }

 private:
  // an array of submeshes that makes up this mesh and materials
  std::vector<Mesh> mMeshes;
  std::vector<Material> mMaterials;

  Transform*
      mTransform;  // Probably not the best way for now, but good for testing
};
