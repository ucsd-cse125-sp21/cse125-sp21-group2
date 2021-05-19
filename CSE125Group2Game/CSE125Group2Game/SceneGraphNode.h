#pragma once

#include "GameObject.h"
#include "Model.h"
#include "ParticleEmitter.h"
#include "Transform.h"

class SceneGraph;

// TODO: I wont be picky, but really each node doesn't need to have a game
// object, i.e. some transition nodes (i.e. root) can just be a transform
class SceneGraphNode {
  friend SceneGraph;

 public:
  // static SceneGraphNode* getRoot();

  GameObject* getObject() const;
  Model* getModel() const;
  std::vector<SceneGraphNode*> getChildren() const;
  SceneGraphNode* getParent();

  // TODO: no transforms in graph sadge
  ParticleEmitter* emitter;

 private:
  SceneGraphNode(SceneGraphNode* parent, GameObject* object);
  SceneGraphNode(SceneGraphNode* parent, GameObject* object, Model* mesh);

  void addChild(SceneGraphNode* child);
  void removeChild(SceneGraphNode* child);

  void update(float delta);

  GameObject* mObject;
  Model* mModel;

  std::vector<SceneGraphNode*> mChildren;
  SceneGraphNode* mParent;
};
