#pragma once

#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <unordered_map>

#include "ServerGraphNode.h"
#include "json.hpp"

class ServerLoader {
 public:
  ServerLoader(std::string fileName);

  std::vector<ServerGraphNode*> mTowers;
  std::vector<ServerGraphNode*> mClouds;

  std::unordered_map<std::string, ServerGraphNode*> mObjects;
};
