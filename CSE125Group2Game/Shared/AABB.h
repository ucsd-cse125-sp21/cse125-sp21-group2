#pragma once
#include <glm/vec3.hpp>

class AABB {
 public:
  // TODO(evan): more ctors?
  AABB() {}
  AABB(glm::vec3 mins, glm::vec3 maxs);

  static AABB FromPosLen(glm::vec3 pos, glm::vec3 lens);

  bool collides(const AABB& other) const;
  bool encompasses(const AABB& other) const;

  glm::vec3 getMins() const { return mMins; }
  glm::vec3 getMaxs() const { return mMaxs; }
  glm::vec3 getLens() const { return mMaxs - mMins; }

 private:
  glm::vec3 mMins, mMaxs;
};
