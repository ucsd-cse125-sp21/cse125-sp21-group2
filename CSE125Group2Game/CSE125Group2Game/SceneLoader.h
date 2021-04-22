#pragma once

#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <unordered_map>

#include "SceneGraphNode.h"
#include "json.hpp"

// actually...
#define ASSET(name) ("Assets/" + std::string(name))

class SceneLoader {
 public:
  SceneLoader(std::string fileName, MeshLoader& loader);

 private:
  std::unordered_map<std::string, SceneGraphNode*> mObjects;
};
