#pragma once

#include "Transform.h"
#include "Mesh.h"

class SceneGraphNode
{
public:
	SceneGraphNode(SceneGraphNode* parent, Transform* transform);

	static SceneGraphNode* getRoot();

private:
	static SceneGraphNode* root;

	Transform* mTransform;
	Mesh* mMesh;

	std::vector<SceneGraphNode*> mChildren;
	SceneGraphNode* mParent;
};