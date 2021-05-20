#pragma once
#include <string>

#include "Model.h"

class ModelLoader {
 public:
  Model* LoadModel(std::string filepath, MeshLoader& loader,
                   TextureLoader& tloader);

 private:
  std::unordered_map<std::string, Model*> mModels;
};
