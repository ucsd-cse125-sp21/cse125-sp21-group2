#pragma once

#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <unordered_map>

#include "ServerGraphNode.h"
#include "json.hpp"

// TODO: will the std::string() get optimized out? interesting question
// actually...

class ServerLoader {
 public:
  ServerLoader(std::string fileName);

 private:
  std::unordered_map<std::string, ServerGraphNode*> mObjects;
};
