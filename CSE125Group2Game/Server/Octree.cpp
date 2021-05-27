#include "Octree.h"

#include "../Shared/Utils.h"
#include "json.hpp"

void OctreeNode::insert(GameObject* object, int depth) {
  // refactor into findNode method
  AABB objBox = object->getTransform()->getBBox();

  if (!mBox.encompasses(objBox)) {
    CRASH("Something went wrong in octree...");
  }

  // at max depth, object goes in this node
  if (depth == Octree::MAX_RECURSION_DEPTH) {
    this->mObjects.emplace_back(object);
    this->mTree.nodeMap.emplace(object, this);
    return;
  }

  OctreeNode* child = getChild(object, true);
  if (child) {
    child->insert(object, depth + 1);
  } else {
    // we aren't encompassed by any object, so we will hold the node
    this->mObjects.emplace_back(object);
    this->mTree.nodeMap.emplace(object, this);
  }
}

void OctreeNode::remove(GameObject* object, int depth) {
  AABB objBox = object->getTransform()->getBBox();

  if (!mBox.encompasses(objBox)) {
    CRASH("Something went wrong in octree...");
  }

  // at max depth, object goes in this node
  if (depth == Octree::MAX_RECURSION_DEPTH) {
    this->mObjects.emplace_back(object);
  }

  OctreeNode* child = getChild(object, true);
  if (child) {
    child->remove(object, depth + 1);
  } else {
    // we aren't encompassed by any object, so we will hold the node
    // erase remove idiom
    this->mObjects.erase(std::remove(mObjects.begin(), mObjects.end(), object));
  }
}

void OctreeNode::update(OctreeNode* root) {
  for (size_t i = 0; i < mObjects.size(); i++) {
    // object must have moved, so we need to remove this object and reinsert
    if (!mBox.encompasses(mObjects[i]->getTransform()->getBBox())) {
      GameObject* obj = mObjects[i];
      mObjects.erase(mObjects.begin() + i);
      mTree.nodeMap.erase(obj);
      i--;
      root->insert(obj, 0);
    }
  }

  for (OctreeNode* child : mChildren) {
    if (child) {
      child->update(root);
    }
  }
}

GameObject* OctreeNode::getCollidingObject(GameObject* object) {
  // check objects
  for (size_t i = 0; i < mObjects.size(); i++) {
    // object must have moved, so we need to remove this object and reinsert
    if (object->shouldNotCollide(mObjects[i])) {
      continue;
    }

    if (object->getTransform()->getBBox().collides(
            (mObjects[i])->getTransform()->getBBox())) {
      return mObjects[i];
    }
  }

  // check children
  GameObject* result = nullptr;
  for (int i = 0; i < 8; i++) {
    if (mChildren[i]) {
      result = mChildren[i]->getCollidingObject(object);
      if (result) {
        return result;
      }
    }
  }

  return nullptr;
}

OctreeNode* OctreeNode::getChild(GameObject* object, bool insert) {
  AABB objBox = object->getTransform()->getBBox();
  std::vector<AABB> octants = divideOctants(mBox);

  // if child is encompassed by a child node, recurse there
  for (int i = 0; i < 8; i++) {
    if (octants[i].encompasses(objBox)) {
      if (this->mChildren[i]) {
        return this->mChildren[i];
      } else if (insert) {
        this->mChildren[i] = new OctreeNode(octants[i], mTree);
        return this->mChildren[i];
      } else {
        return nullptr;
      }
    }
  }

  return nullptr;
}

std::vector<AABB> OctreeNode::divideOctants(const AABB& box) const {
  glm::vec3 halfLens = box.getLens() / 2.0f;
  glm::vec3 mins = box.getMins();
  glm::vec3 maxs = box.getMaxs();
  glm::vec3 halfs = mins + halfLens;

  float minLeft = mins.x, maxLeft = halfs.x;
  float minRight = halfs.x, maxRight = maxs.x;
  float minLow = mins.y, maxLow = halfs.y;
  float minUp = halfs.y, maxUp = maxs.y;
  float minBack = mins.z, maxBack = halfs.z;
  float minFront = halfs.z, maxFront = maxs.z;

  std::vector<AABB> octants = {
      // left low back
      AABB(glm::vec3(minLeft, minLow, minBack),
           glm::vec3(maxLeft, maxLow, maxBack)),
      // right low back
      AABB(glm::vec3(minRight, minLow, minBack),
           glm::vec3(maxRight, maxLow, maxBack)),
      // left up back
      AABB(glm::vec3(minLeft, minUp, minBack),
           glm::vec3(maxLeft, maxUp, maxBack)),
      // right up back
      AABB(glm::vec3(minRight, minUp, minBack),
           glm::vec3(maxRight, maxUp, maxBack)),
      // left low front
      AABB(glm::vec3(minLeft, minLow, minFront),
           glm::vec3(maxLeft, maxLow, maxFront)),
      // right low front
      AABB(glm::vec3(minRight, minLow, minFront),
           glm::vec3(maxRight, maxLow, maxFront)),
      // left up front
      AABB(glm::vec3(minLeft, minUp, minFront),
           glm::vec3(maxLeft, maxUp, maxFront)),
      // right up front
      AABB(glm::vec3(minRight, minUp, minFront),
           glm::vec3(maxRight, maxUp, maxFront)),
  };

  assert(octants.size() == 8);
  return std::move(octants);
}

// allow setting box size w/e for now
Octree::Octree()
    : mRoot(AABB::FromPosLen(glm::vec3(0, 0, 0), glm::vec3(100, 100, 100)),
            *this) {}

Octree::~Octree() {}

void Octree::insert(GameObject* object) { mRoot.insert(object, 0); }

//  for each object in the tree, if not in this node anymore,
// then reinsert into the tree
// probably can optimize this to some degree... but meh
void Octree::update() { mRoot.update(&mRoot); }

GameObject* Octree::getCollidingObject(GameObject* object) {
  return mRoot.getCollidingObject(object);
}

void Octree::remove(GameObject* object) {
  OctreeNode* node = nodeMap[object];
  node->mObjects.erase(
      std::remove(node->mObjects.begin(), node->mObjects.end(), object));
  nodeMap.erase(object);
}
