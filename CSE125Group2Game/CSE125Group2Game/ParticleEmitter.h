#pragma once
#include <functional>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "Camera.h"
#include "TextureLoader.h"

#define RAND_FLOAT_NORM() \
  static_cast<float>(rand()) / static_cast<float>(RAND_MAX)

// for now, just a point basically
struct Particle {
  glm::vec3 position;
  glm::vec3 color = glm::vec3(1.0f);
  // TODO: might be better to do mark and sweep style, moving alive particles to
  // front,
  //       then saying "hey only render blah" number particles
  glm::vec2 angle;
  float speed;

  // Value between 1.0 and 0.0 that signifies how long the particle has left to
  // live
  float life;
};

// TODO: going to need to clean these up when they are done, or we could have
// performance issues I guess
class ParticleEmitter {
 public:
  ParticleEmitter(Texture particleTexture)
      : ParticleEmitter(particleTexture, 100) {}
  ParticleEmitter(Texture particleTexture, size_t numParticles);

  // these will be virtual/overriden
  virtual void InitializeParticle(Particle& particle);
  virtual void UpdateParticle(Particle& particle, float delta);

  void Update(float delta);

  // refactor into Render Manager honestly
  // maybe visitor pattern or something...
  void Draw(TextureLoader& tloader, const glm::mat4& cameraMatrix);

  // gross, public...
  unsigned int mVao;
  unsigned int mUVVBO;
  unsigned int mParticleVBO;

  Texture mParticleTexture;
  std::vector<Particle> mParticles;

  // TODO: for now just always have 100 particles
  size_t mNumParticles = 100;

  // TODO: this will say whether its one and done (i.e. explosion) or keep going
  // (i.e. ship flame)
  bool mIsContinuous = false;
  bool mAnealSize = true;
  float mParticleSize = 4.0f;
  bool isRainbow = false;
  float mParticleSpeed = 5.0f;
  bool mShouldRender = true;

  bool isDone() const;
};
