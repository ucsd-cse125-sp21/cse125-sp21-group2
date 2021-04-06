#include "SceneGraphNode.h"

SceneGraphNode* SceneGraphNode::getRoot() {
    if (!root) {
        root = new SceneGraphNode(NULL, new Transform(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)));
    }

    return root;
}

SceneGraphNode::SceneGraphNode(SceneGraphNode* parent, Transform* transform) :
    mParent(parent), mTransform(transform), mMesh(NULL) {}