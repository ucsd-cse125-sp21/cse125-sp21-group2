#pragma once

#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <unordered_map>

#include "SceneGraphNode.h"
#include "json.hpp"

// TODO: will the std::string() get optimized out? interesting question
// actually...
#define ASSET(name) ("Assets/" + std::string(name))

class SceneLoader {
 public:
  SceneLoader(std::string fileName, MeshLoader& loader);

 private:
  std::unordered_map<std::string, SceneGraphNode*> mObjects;
};
