#include "SceneGraph.h"

SceneGraph::SceneGraph()
    : mpRoot(std::make_unique<SceneGraphNode>(
          nullptr,
          new GameObject(new Transform(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0),
                                       glm::vec3(1, 1, 1)),
                         const_cast<char*>("root0000"), 0))) {}

SceneGraphNode* SceneGraph::getRoot() const { return mpRoot.get(); }

SceneGraphNode* SceneGraph::getByName(const std::string& name) const {
  auto value = mNameMapping.find(name);
  return value != mNameMapping.end() ? value->second : nullptr;
}
