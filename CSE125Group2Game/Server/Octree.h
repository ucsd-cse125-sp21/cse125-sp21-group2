#pragma once
#include <iostream>
#include <stack>
#include <vector>

#include "GameObject.h"

class Octree;

class OctreeNode {
 public:
  OctreeNode(const AABB& box, Octree& tree) : mBox(box), mTree(tree) {}
  // todo(evan): ~OctreeNode
  void insert(GameObject* object, int depth);
  void remove(GameObject* object, int depth);
  void update(OctreeNode* root);
  GameObject* getCollidingObject(GameObject* object);
  OctreeNode* getChild(GameObject* object, bool insert);

  OctreeNode* mChildren[8];
  // objects that are minimally contained by this node, i.e. they don't fit
  // into a smaller child node
  std::vector<GameObject*> mObjects;
  AABB mBox;

  // reference to main tree
  Octree& mTree;

  // helpers
  std::vector<AABB> divideOctants(const AABB& box) const;
};

// lets just do this recursively for now:
// Meh kinda annoying, cause going to have to remove objects from here when
// deleted... something automatic would be much nicer
class Octree {
 public:
  class iterator {
   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = GameObject*;
    using difference_type = std::ptrdiff_t;
    using pointer = const GameObject**;
    using reference = GameObject*;

    explicit iterator(GameObject* object, OctreeNode* node)
        : mpCollider(object), mpCurrentNode(node), mCurrIndex(-1) {
      mToVisit.reserve(100);
      if (node) {
        mToVisit.push_back(node);
        ++(*this);
      }
    }

    iterator& operator++() {
      if (mCurrIndex > 0 && mCurrIndex < mpCurrentNode->mObjects.size()) {
        mpCurrentGameObject = mpCurrentNode->mObjects[mCurrIndex];
        mCurrIndex++;
        return *this;
      }

      do {
        // nothing left, cause current node has nothing
        if (mToVisit.empty()) {
          mpCurrentNode = nullptr;
          return *this;
        }

        mCurrIndex = 0;
        mpCurrentNode = mToVisit.back();
        mToVisit.pop_back();

        // put all intersecting children into visit queue
        for (size_t i = 0; i < 8; i++) {
          OctreeNode* child = mpCurrentNode->mChildren[i];
          // if the child collides with the bounding box, then we need to
          // check it
          if (child &&
              mpCollider->getTransform()->getBBox().collides(child->mBox)) {
            mToVisit.push_back(child);
          }
        }
      } while (mpCurrentNode->mObjects.empty());

      // set first item from node
      mpCurrentGameObject = mpCurrentNode->mObjects[mCurrIndex];
      mCurrIndex++;
      return *this;
    }
    iterator operator++(int) {
      iterator retval = *this;
      ++(*this);
      return retval;
    }
    bool operator==(iterator other) const {
      if (!mpCurrentNode && !other.mpCurrentNode) {
        return true;
      }
      return (mpCurrentNode == other.mpCurrentNode) &&
             (mCurrIndex == other.mCurrIndex);
    }
    bool operator!=(iterator other) const { return !(*this == other); }
    reference operator*() const { return mpCurrentGameObject; }

   private:
    GameObject* mpCollider;

    // TODO: invariants
    GameObject* mpCurrentGameObject;
    OctreeNode* mpCurrentNode;
    int mCurrIndex;

    // new gameplan
    std::vector<OctreeNode*> mToVisit;
  };

  Octree();
  ~Octree();

  void insert(GameObject* object);
  void update();
  void remove(GameObject* object);
  GameObject* getCollidingObject(GameObject* object);

  // iterator stuff
  iterator begin(GameObject* object) { return iterator(object, &mRoot); }
  iterator end() { return iterator(nullptr, nullptr); }

  // TODO: collision stuff

  static const int MAX_RECURSION_DEPTH = 10;

 private:
  friend OctreeNode;
  // children
  // invariant: must always be 8 in size
  // invariant: box of child must be 1/2 length box of parent!
  OctreeNode mRoot;
  AABB mBox;  // TODO: best way? we could also just divide the box as we
              // recurse down

  // todo(evan): kinda hacky
  std::unordered_map<GameObject*, OctreeNode*> nodeMap;
};
