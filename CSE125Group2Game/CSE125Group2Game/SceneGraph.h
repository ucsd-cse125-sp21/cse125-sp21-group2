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

  static SceneGraph FromFile(const std::string& fileName, MeshLoader& loader,
                             TextureLoader& tloader);

  SceneGraphNode* getRoot() const;
  SceneGraphNode* getByName(const std::string& name) const;

  SceneGraphNode* addChild(GameObject* obj, Model* model);
  SceneGraphNode* addChild(GameObject* obj, Model* model,
                           SceneGraphNode* parent);

  void removeChild(SceneGraphNode* node);

 private:
  std::unique_ptr<SceneGraphNode> mpRoot;
  // This only gets populated by SceneLoader rn.... we can fix that by changing
  // SceneGraphNode but w/e
  std::unordered_map<std::string, SceneGraphNode*> mNameMapping;
};
