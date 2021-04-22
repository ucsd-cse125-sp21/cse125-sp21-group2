#pragma once

#include "GameObject.h"
#include "Model.h"
#include "Transform.h"

class SceneGraphNode {
 public:
  SceneGraphNode(SceneGraphNode* parent, GameObject* object);

  SceneGraphNode(SceneGraphNode* parent, GameObject* object, Model* mesh);

  static SceneGraphNode* getRoot();

  void addChild(SceneGraphNode* child);

  void removeChild(SceneGraphNode* child);

  GameObject* getObject();

  Model* getMesh();

  std::vector<SceneGraphNode*> getChildren();

  SceneGraphNode* getParent();

 private:
  static SceneGraphNode* root;

  GameObject* mObject;
  Model* mMesh;

  std::vector<SceneGraphNode*> mChildren;
  SceneGraphNode* mParent;
};
