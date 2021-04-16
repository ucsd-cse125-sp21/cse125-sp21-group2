﻿#include "ServerGraphNode.h"

ServerGraphNode* ServerGraphNode::root;

ServerGraphNode::ServerGraphNode(ServerGraphNode* parent, Transform* transform)
    : mParent(parent), mTransform(transform) {
  if (parent != NULL) {
    parent->addChild(this);
  }
}

ServerGraphNode* ServerGraphNode::getRoot() {
  if (!root) {
    // Create a root node at (0,0,0) with no rotation and 1x scale
    root = new ServerGraphNode(
        NULL, new Transform(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0),
                            glm::vec3(1, 1, 1)));
  }

  return root;
}

void ServerGraphNode::addChild(ServerGraphNode* child) {
  mChildren.push_back(child);
}

void ServerGraphNode::removeChild(ServerGraphNode* child) {
  // Loop through every child to remove the correct child
  for (auto i = mChildren.begin(); i != mChildren.end(); i++) {
    if ((*i) == child) {
      mChildren.erase(i);
      break;
    }
  }

  return;
}

Transform* ServerGraphNode::getTransform() { return mTransform; }

std::vector<ServerGraphNode*> ServerGraphNode::getChildren() {
  return mChildren;
}

ServerGraphNode* ServerGraphNode::getParent() { return mParent; }

std::vector<GameObject*> ServerGraphNode::makeWorld(glm::vec3 cPosition,
                                                    glm::quat cRotation,
                                                    glm::vec3 cScale) {
  std::vector<GameObject*> result;

  // For every child, recurse
  for (int i = 0; i < mChildren.size(); i++) {
    std::vector<GameObject*> temp = makeWorld(
        cPosition + mTransform->getTranslation(),
        cRotation * mTransform->getRotation(), cScale * mTransform->getScale());

    // Add childrens result to vector
    result.insert(result.end(), temp.begin(), temp.end());
  }

  Transform* transform =
      new Transform(cPosition + mTransform->getTranslation(),
                    cRotation * mTransform->getRotation(),
                    cScale * mTransform->getScale(), mTransform->getBBox());

  result.push_back(new GameObject(transform, mName, 10));
  return result;
}

void ServerGraphNode::setName(std::string name) { mName = name; }
std::string ServerGraphNode::getName() { return mName; }
