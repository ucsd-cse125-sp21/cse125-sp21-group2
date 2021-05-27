#pragma once
#include "ParticleEmitter.h"
class ConeParticleEmitter : public ParticleEmitter {
 public:
  ConeParticleEmitter(Texture particleTexture, glm::vec3 direction,
                      float spread)
      : ParticleEmitter(particleTexture, 100),
        mDirection(direction),
        mSpread(spread) {}
  ConeParticleEmitter(Texture particleTexture, glm::vec3 direction,
                      float spread, size_t numParticles)
      : ParticleEmitter(particleTexture, numParticles),
        mDirection(direction),
        mSpread(spread) {}

  // these will be virtual/overriden
  void InitializeParticle(Particle& particle) override;
  void UpdateParticle(Particle& particle, float delta) override;

 private:
  glm::vec3 mDirection;
  float mSpread;  // angle inside cone
};
