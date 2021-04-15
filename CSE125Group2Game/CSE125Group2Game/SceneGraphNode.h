﻿#pragma once

#include "Model.h"
#include "Transform.h"

class SceneGraphNode {
 public:
  SceneGraphNode(SceneGraphNode* parent, Transform* transform);

  SceneGraphNode(SceneGraphNode* parent, Transform* transform, Model* mesh);

  static SceneGraphNode* getRoot();

  void addChild(SceneGraphNode* child);

  void removeChild(SceneGraphNode* child);

  Transform* getTransform();

  Model* getMesh();

  std::vector<SceneGraphNode*> getChildren();

  SceneGraphNode* getParent();

 private:
  static SceneGraphNode* root;

  Transform* mTransform;
  Model* mMesh;

  std::vector<SceneGraphNode*> mChildren;
  SceneGraphNode* mParent;
};
