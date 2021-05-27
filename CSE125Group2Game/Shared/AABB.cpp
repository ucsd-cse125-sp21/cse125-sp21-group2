#include "AABB.h"

AABB::AABB(glm::vec3 mins, glm::vec3 maxs) : mMins(mins), mMaxs(maxs) {}

AABB AABB::FromPosLen(glm::vec3 pos, glm::vec3 lens) {
  glm::vec3 mins = pos - lens / 2.0f;
  glm::vec3 maxs = pos + lens / 2.0f;
  return AABB(mins, maxs);
}

bool AABB::collides(const AABB& other) const {
  return (this->mMins.x <= other.mMaxs.x && this->mMaxs.x >= other.mMins.x) &&
         (this->mMins.y <= other.mMaxs.y && this->mMaxs.y >= other.mMins.y) &&
         (this->mMins.z <= other.mMaxs.z && this->mMaxs.z >= other.mMins.z);
}

bool AABB::encompasses(const AABB& other) const {
  return (this->mMins.x <= other.mMins.x && this->mMaxs.x >= other.mMaxs.x) &&
         (this->mMins.y <= other.mMins.y && this->mMaxs.y >= other.mMaxs.y) &&
         (this->mMins.z <= other.mMins.z && this->mMaxs.z >= other.mMaxs.z);
}
