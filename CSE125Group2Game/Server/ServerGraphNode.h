#pragma once

#include "GameObject.h"
#include "Transform.h"

class ServerGraphNode {
 public:
  ServerGraphNode(ServerGraphNode* parent, Transform* transform);

  static ServerGraphNode* getRoot();

  void addChild(ServerGraphNode* child);

  void removeChild(ServerGraphNode* child);

  Transform* getTransform();

  std::vector<ServerGraphNode*> getChildren();

  ServerGraphNode* getParent();

  void setName(char* name);
  char* getName();

  std::vector<GameObject*> makeWorld(glm::vec3 cPosition, glm::quat cRotation,
                                     glm::vec3 cScale);

 private:
  static ServerGraphNode* root;

  Transform* mTransform;
  std::vector<ServerGraphNode*> mChildren;
  ServerGraphNode* mParent;

  char mName[NAME_LEN];
};
