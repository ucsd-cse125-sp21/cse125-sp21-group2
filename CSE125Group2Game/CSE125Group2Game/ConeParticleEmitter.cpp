#include "ConeParticleEmitter.h"

void ConeParticleEmitter::InitializeParticle(Particle& particle) {
  float theta = (RAND_FLOAT_NORM() - 0.5) * mSpread;
  float phi = (RAND_FLOAT_NORM() - 0.5) * mSpread;
  float speed = RAND_FLOAT_NORM() * mParticleSpeed + 10.0f;
  float life = RAND_FLOAT_NORM() * 1.0f;

  particle.position = glm::vec3(0.0);
  particle.color = glm::vec3(RAND_FLOAT_NORM(), RAND_FLOAT_NORM(),
                             RAND_FLOAT_NORM());  // TODO: colors less random
  particle.angle = glm::vec2(theta, phi);
  particle.speed = speed;
  particle.life = life;
}

void ConeParticleEmitter::UpdateParticle(Particle& particle, float delta) {
  particle.life -= 0.5f * delta;

  float theta = glm::radians(particle.angle[0]);
  float phi = glm::radians(particle.angle[1]);
  float dPos = particle.speed * delta;
  // correct?
  particle.position +=
      dPos * (mDirection + glm::vec3(sin(theta) * cos(phi),
                                     sin(theta) * sin(phi), cos(theta)));
}
