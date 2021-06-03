#include "SceneGraphNode.h"

// TODO: This interface is kinda weird. It isn't very clear that this ctor also
// adds the new node into the child list of the parent.
SceneGraphNode::SceneGraphNode(SceneGraphNode* parent, GameObject* object)
    : mParent(parent), mObject(object), mpModels(0) {}

SceneGraphNode::SceneGraphNode(SceneGraphNode* parent, GameObject* object,
                               Model* model)
    : mParent(parent), mObject(object), mpModels(0) {
  mpModels.emplace_back(model);
}

SceneGraphNode::SceneGraphNode(SceneGraphNode* parent, GameObject* object,
                               std::vector<Model*>&& models)
    : mParent(parent), mObject(object), mpModels(std::move(models)) {}

void SceneGraphNode::addChild(SceneGraphNode* child) {
  mChildren.push_back(child);
}

// TODO: remove children of child being removed
void SceneGraphNode::removeChild(SceneGraphNode* child) {
  // Loop through every child to remove the correct child
  for (auto i = mChildren.begin(); i != mChildren.end(); i++) {
    if ((*i) == child) {
      mChildren.erase(i);
      break;
    }
  }

  return;
}

GameObject* SceneGraphNode::getObject() const { return mObject; }

Model* SceneGraphNode::getModel() const {
  if (!mObject) {
    return nullptr;
  }

  if (mObject->getModelIndex() >= mpModels.size()) {
    return nullptr;
  }

  return mpModels[mObject->getModelIndex()];
}

std::vector<SceneGraphNode*> SceneGraphNode::getChildren() const {
  return mChildren;
}

SceneGraphNode* SceneGraphNode::getParent() { return mParent; }

void SceneGraphNode::update(float delta) {
  if (emitter) {
    emitter->Update(delta);
  }

  for (int i = 0; i < mChildren.size(); i++) {
    auto child = mChildren[i];
    child->update(delta);

    if ((!child->emitter || child->emitter->isDone()) &&
        child->mChildren.empty() && child->getObject()->shouldDelete()) {
      removeChild(child);
      i--;
    }
  }
}
