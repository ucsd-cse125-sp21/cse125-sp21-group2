#include "SceneGraphNode.h"

SceneGraphNode* SceneGraphNode::root;

SceneGraphNode::SceneGraphNode(SceneGraphNode* parent, GameObject* object)
    : mParent(parent), mObject(object), mMesh(NULL) {
  if (parent != NULL) {
    parent->addChild(this);
  }
}

SceneGraphNode::SceneGraphNode(SceneGraphNode* parent, GameObject* object,
                               Model* mesh)
    : mParent(parent), mObject(object), mMesh(mesh) {
  if (parent != NULL) {
    parent->addChild(this);
  }
}

SceneGraphNode* SceneGraphNode::getRoot() {
  if (!root) {
    // Create a root node at (0,0,0) with no rotation and 1x scale
    root = new SceneGraphNode(
        NULL,
        new GameObject(new Transform(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0),
                                     glm::vec3(1, 1, 1)),
                       (char*)"root0000", 0));
  }

  return root;
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

GameObject* SceneGraphNode::getObject() { return mObject; }

Model* SceneGraphNode::getMesh() { return mMesh; }

std::vector<SceneGraphNode*> SceneGraphNode::getChildren() { return mChildren; }

SceneGraphNode* SceneGraphNode::getParent() { return mParent; }
