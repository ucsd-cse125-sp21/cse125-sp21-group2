﻿#pragma once

#include "GameObject.h"
#include "Mesh.h"
#include "Transform.h"

class SceneGraphNode {
 public:
  SceneGraphNode(SceneGraphNode* parent, Transform* transform);

  SceneGraphNode(SceneGraphNode* parent, Transform* transform, Mesh* mesh);

  static SceneGraphNode* getRoot();

  void addChild(SceneGraphNode* child);

  void removeChild(SceneGraphNode* child);

  Transform* getTransform();

  Mesh* getMesh();

  std::vector<SceneGraphNode*> getChildren();

  SceneGraphNode* getParent();

  void setName(char* name);
  char* getName();

  std::vector<GameObject*> makeWorld(glm::vec3 cPosition, glm::quat cRotation,
                                     glm::vec3 cScale);

 private:
  static SceneGraphNode* root;

  Transform* mTransform;
  Mesh* mMesh;

  std::vector<SceneGraphNode*> mChildren;
  SceneGraphNode* mParent;

  char mName[NAME_LEN];
};
