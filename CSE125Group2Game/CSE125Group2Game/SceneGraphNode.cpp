#include "SceneGraphNode.h"

SceneGraphNode* SceneGraphNode::root;

SceneGraphNode::SceneGraphNode(SceneGraphNode* parent, Transform* transform)
    : mParent(parent), mTransform(transform), mMesh(NULL) {
  if (parent != NULL) {
    parent->addChild(this);
  }
}

SceneGraphNode::SceneGraphNode(SceneGraphNode* parent, Transform* transform,
                               Mesh* mesh)
    : mParent(parent), mTransform(transform), mMesh(mesh) {
  if (parent != NULL) {
    parent->addChild(this);
  }
}

SceneGraphNode* SceneGraphNode::getRoot() {
  if (!root) {
    // Create a root node at (0,0,0) with no rotation and 1x scale
    root = new SceneGraphNode(
        NULL, new Transform(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0),
                            glm::vec3(1, 1, 1)));
  }

  return root;
}

void SceneGraphNode::addChild(SceneGraphNode* child) {
  mChildren.push_back(child);
}

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

Transform* SceneGraphNode::getTransform() { return mTransform; }

Mesh* SceneGraphNode::getMesh() { return mMesh; }

std::vector<SceneGraphNode*> SceneGraphNode::getChildren() { return mChildren; }

SceneGraphNode* SceneGraphNode::getParent() { return mParent; }
