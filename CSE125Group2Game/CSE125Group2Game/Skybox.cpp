#include "Skybox.h"
Skybox::Skybox(const std::string& directory, MeshLoader& mLoader,
               TextureLoader& tLoader) {
  mpCube = Model::Cube(mLoader);
  mCubeMap = tLoader.loadCubeMap(directory);
}
