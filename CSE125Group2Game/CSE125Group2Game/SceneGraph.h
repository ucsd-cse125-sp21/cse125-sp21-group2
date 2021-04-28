#pragma once
#include <cstddef>
#include <iterator>
#include <memory>
#include <string>

#include "SceneGraphNode.h"

class SceneLoader;

class SceneGraph {
 public:
  SceneGraph();
  SceneGraphNode* getRoot() const;
  SceneGraphNode* getByName(const std::string& name) const;

 private:
  // HACK: Gross but makes things easy for now
  friend SceneLoader;

  std::unique_ptr<SceneGraphNode> mpRoot;
  // This only gets populated by SceneLoader rn.... we can fix that by changing
  // SceneGraphNode but w/e
  std::unordered_map<std::string, SceneGraphNode*> mNameMapping;
};
