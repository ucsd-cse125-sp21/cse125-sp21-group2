#include "SceneGraphNode.h"

// TODO: This interface is kinda weird. It isn't very clear that this ctor also
// adds the new node into the child list of the parent.
SceneGraphNode::SceneGraphNode(SceneGraphNode* parent, GameObject* object)
    : mParent(parent), mObject(object), mModel(NULL) {
  if (parent != NULL) {
    parent->addChild(this);
  }
}

SceneGraphNode::SceneGraphNode(SceneGraphNode* parent, GameObject* object,
                               Model* model)
    : mParent(parent), mObject(object), mModel(model) {
  if (parent != NULL) {
    parent->addChild(this);
  }
}

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

Model* SceneGraphNode::getModel() const { return mModel; }

std::vector<SceneGraphNode*> SceneGraphNode::getChildren() const {
  return mChildren;
}

SceneGraphNode* SceneGraphNode::getParent() { return mParent; }
