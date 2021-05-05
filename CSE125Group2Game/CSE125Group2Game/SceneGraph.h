#pragma once
#include <cstddef>
#include <iterator>
#include <memory>
#include <optional>
#include <string>

#include "Camera.h"
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

  Camera& addCamera();
  Camera& addCamera(SceneGraphNode* node);

  void removeChild(SceneGraphNode* node);

  std::optional<glm::mat4> getViewMatrix() const;

 private:
  std::unique_ptr<SceneGraphNode> mpRoot;

  // TODO: Gross but works. We just store the camera "node" in here along with
  // the camera. A much cleaner way to implement this is to refactor
  //       objects that can be stored in scene graph nodes as an interface or
  //       something, but that will add some complexity
  Camera mCamera;
  SceneGraphNode* cameraNode;
  // This only gets populated by SceneLoader rn.... we can fix that by changing
  // SceneGraphNode but w/e
  std::unordered_map<std::string, SceneGraphNode*> mNameMapping;
};
