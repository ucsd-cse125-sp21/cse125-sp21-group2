#pragma once
#include "Model.h"

// Represents a skybox
class Skybox {
 public:
  Skybox(const std::string& directory, MeshLoader& mLoader,
         TextureLoader& tLoader);

  // technically overkill, don't need materials, just mesh. but let us use
  // Model::Cube.
  Model* mpCube;
  Texture mCubeMap;
};
