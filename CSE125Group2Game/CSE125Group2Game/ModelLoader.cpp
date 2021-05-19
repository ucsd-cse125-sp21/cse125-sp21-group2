#include "ModelLoader.h"

Model* ModelLoader::LoadModel(std::string filepath, MeshLoader& loader,
                              TextureLoader& tloader) {
  auto modelIter = mModels.find(filepath);
  if (modelIter != mModels.end()) {
    return modelIter->second;
  }

  // model not loaded yet
  Model* model = new Model(filepath, loader, tloader);
  mModels[filepath] = model;
  return model;
}
