﻿#pragma once

#include "GameObject.h"
#include "Model.h"
#include "Transform.h"

// TODO: I wont be picky, but really each node doesn't need to have a game
// object, i.e. some transition nodes (i.e. root) can just be a transform
class SceneGraphNode {
 public:
  SceneGraphNode(SceneGraphNode* parent, GameObject* object);
  SceneGraphNode(SceneGraphNode* parent, GameObject* object, Model* mesh);

  // static SceneGraphNode* getRoot();

  void addChild(SceneGraphNode* child);

  void removeChild(SceneGraphNode* child);

  GameObject* getObject() const;

  Model* getModel() const;

  std::vector<SceneGraphNode*> getChildren() const;

  SceneGraphNode* getParent();

  const Transform& getTransform() const { return mTransform; };
  void setTransform(const Transform& transform) { mTransform = transform; }

 private:
  GameObject* mObject;
  Model* mModel;

  Transform mTransform;

  std::vector<SceneGraphNode*> mChildren;
  SceneGraphNode* mParent;
};
