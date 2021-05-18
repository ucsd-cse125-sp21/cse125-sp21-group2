#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

#include "Material.h"
#include "MeshLoader.h"
#include "Transform.h"

class ModelLoader;

class Model {
 public:
  static Model* Cube(MeshLoader& loader);

  Model(const std::vector<Vertex>& vertices,
        const std::vector<glm::uvec3>& indices, MeshLoader& loader);

  // TODO: may want to refactor this for better encapsulation...
  const std::vector<Mesh>& meshes() const { return mMeshes; }
  const std::vector<Material>& materials() const { return mMaterials; }

  // TODO: refacotor this yay
  std::vector<Material> mMaterials;

 private:
  friend ModelLoader;
  Model(const std::string& filePath, MeshLoader& loader,
        TextureLoader& tloader);
  // an array of submeshes that makes up this mesh and materials
  std::vector<Mesh> mMeshes;
};
