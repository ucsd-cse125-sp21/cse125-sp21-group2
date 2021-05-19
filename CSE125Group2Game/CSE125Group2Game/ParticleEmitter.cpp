#include "ParticleEmitter.h"

#include <glad/glad.h>

#include <algorithm>
#include <glm/vec2.hpp>

#include "Camera.h"

#define RAND_FLOAT_NORM() \
  static_cast<float>(rand()) / static_cast<float>(RAND_MAX)

ParticleEmitter::ParticleEmitter(Texture particleTexture, size_t numParticles)
    : mParticleTexture(particleTexture),
      mNumParticles(numParticles),
      mParticles(numParticles, Particle()) {
  // init particles
  for (Particle& particle : mParticles) {
    InitializeParticle(particle);
    assert(particle.life >= 0 && particle.life <= 1,
           "Particle life out side of bounds!");
  }

  glGenVertexArrays(1, &mVao);
  glBindVertexArray(mVao);

  glGenBuffers(1, &mUVVBO);
  glGenBuffers(1, &mParticleVBO);

  // simple quad mesh, should really just do in MeshLoader or something
  glm::vec2 uvs[4] = {
      glm::vec2(0, 0),
      glm::vec2(1, 0),
      glm::vec2(0, 1),
      glm::vec2(1, 1),
  };

  // bind the UV buffer
  glBindBuffer(GL_ARRAY_BUFFER, mUVVBO);
  glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), uvs, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(0);

  // bind the particle buffer
  glBindBuffer(GL_ARRAY_BUFFER, mParticleVBO);
  glBufferData(GL_ARRAY_BUFFER, numParticles * sizeof(Particle), 0,
               GL_STREAM_DRAW);

  // setup vertex attributes
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), nullptr);
  glVertexAttribDivisor(1, 1);
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle),
                        (void*)offsetof(Particle, life));
  glVertexAttribDivisor(2, 1);
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);
}

void ParticleEmitter::Update(float delta) {
  // lets just move them up a random bit
  // TODO: fix this. Have each particle have an angle? then move along angle.
  // respawn when dead
  int i = 0;
  for (int i = 0; i < mParticles.size(); i++) {
    Particle& particle = mParticles[i];
    UpdateParticle(particle, delta);
    if (particle.life <= 0.0f) {
      if (mIsContinuous) {
        InitializeParticle(particle);
        assert(particle.life >= 0 && particle.life <= 1,
               "Particle life out side of bounds!");
      } else {
        mParticles.erase(mParticles.begin() + i);
        i--;
      }
    }
  }
}

void ParticleEmitter::InitializeParticle(Particle& particle) {
  float theta = RAND_FLOAT_NORM() * 360;
  float phi = RAND_FLOAT_NORM() * 360;
  float speed = RAND_FLOAT_NORM() * 5.0f;
  float life = RAND_FLOAT_NORM() * 1.0f;

  particle.position = glm::vec3(0.0);
  particle.angle = glm::vec2(theta, phi);
  particle.speed = speed;
  particle.life = life;
}

void ParticleEmitter::UpdateParticle(Particle& particle, float delta) {
  particle.life -= 0.25f * delta;

  float theta = glm::radians(particle.angle[0]);
  float phi = glm::radians(particle.angle[1]);
  float dPos = particle.speed * delta;
  particle.position +=
      glm::vec3(dPos * sin(theta) * cos(phi), dPos * sin(theta) * sin(phi),
                dPos * cos(theta));
}

// TODO: Refactor out into other method
void ParticleEmitter::Draw(TextureLoader& tloader,
                           const glm::mat4& cameraMatrix) {}

bool ParticleEmitter::isDone() const { return mParticles.empty(); }
