#include "pond/motor.h"
#include "pond/log.h"
#include "pond/organism.h"
#include "pond/painter.h"
#include "pond/types.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

Motor *motor_create(MotorKind kind, Organism *organism,
                    Vector2 startDisplacement, Vector2 startVelocity) {
  Motor *m = (Motor *)calloc(1, sizeof(Motor));
  m->self = organism;
  m->displacement = startDisplacement;
  m->velocity = startVelocity;
  m->acceleration = (Vector2){0.0f, 0.0f};
  m->timeOffset = pond_random(0.0f, CLOCK_MAX);
  m->kind = kind;
  m->velocityPropagation = startVelocity;
  m->velocityNormal = startVelocity;
  m->algaeBeSlow = 0.2f;
  return m;
}

void motor_update(Motor *m) {
  assert(m->self != NULL);

  motor_bounceIfOrganismCollision(m);

  if (m->kind == MOTOR_KIND_SNAKE) {
    motor_bounceIfWallCollisionSnake(m);
  } else {
    motor_bounceIfWallCollision(m);
  }

  motor_move(m);
  motor_clampToWall(m);
}

void motor_move(Motor *m) {
  // Update acceleration
  Vector2 accelerationStep = (Vector2){0.0f, 0.0f};
  accelerationStep = pond_vadd(
      accelerationStep,
      pond_vmult(pond_vrandom2D(), ACCELERATION_NOISE_FACTOR * 12.0f *
                                       sinf(g_clock + m->timeOffset)));
  accelerationStep = pond_vdiv(accelerationStep, powf(m->self->mass, 3));
  Vector2 acceleration =
      pond_vmult(m->acceleration, ACCELERATION_GROWTH_FACTOR * 0.5f);
  acceleration = pond_vadd(acceleration, accelerationStep);
  m->acceleration = acceleration;

  if (m->kind == MOTOR_KIND_ALGAE) {
    motor_updateAccelerationAlgae(m);
  }
  // Update velocity
  Vector2 velocity = m->velocity;
  velocity = pond_vadd(velocity, m->acceleration);
  float mag = pond_vmag(velocity);
  if (mag > TERMINAL_VELOCITY) {
    m->velocity = pond_vdiv(velocity, mag / TERMINAL_VELOCITY);
  } else if (mag < STAGNANT_VELOCITY) {
    m->velocity = pond_vmult(velocity, STAGNANT_VELOCITY / mag);
  }

  if (m->kind == MOTOR_KIND_SNAKE) {
    motor_updateVelocitySnake(m);
  } else if (m->kind == MOTOR_KIND_ALGAE) {
    motor_updateVelocityAlgae(m);
  }
  // Update displacement
  m->displacement = pond_vadd(m->displacement, m->velocity);
}

void motor_updateVelocitySnake(Motor *m) {
  Vector2 velocityPropagation = m->velocity;
  velocityPropagation = pond_vadd(velocityPropagation, m->acceleration);
  m->velocityPropagation = velocityPropagation;

  Vector2 rotated = (Vector2){-m->velocityNormal.y, m->velocityNormal.x};
  m->velocityNormal = pond_vmult(rotated, sinf(g_clock + m->timeOffset));
  m->velocityNormal = pond_vmult(m->velocityNormal, SLITHER_FACTOR);
  Vector2 v = pond_vadd(m->velocityPropagation, m->velocityNormal);
  m->velocity = v;

  float mag = pond_vmag(m->velocity);
  if (mag > TERMINAL_VELOCITY * 2) {
    m->velocity = pond_vdiv(m->velocity, mag / (TERMINAL_VELOCITY * 2));
  } else if (mag < STAGNANT_VELOCITY * 2) {
    m->velocity = pond_vmult(m->velocity, (STAGNANT_VELOCITY * 2) / mag);
  }
}

void motor_updateVelocityAlgae(Motor *m) {
  Vector2 v = m->velocity;
  v = pond_vadd(v, m->acceleration);

  float mag = pond_vmag(v);
  if (mag == 0.0f) {
    return;
  }

  float algaeBeSlow = 0.1f;

  if (mag > TERMINAL_VELOCITY * algaeBeSlow) {
    m->velocity = pond_vdiv(v, mag / (TERMINAL_VELOCITY * algaeBeSlow));
  } else if (mag < STAGNANT_VELOCITY * algaeBeSlow) {
    m->velocity = pond_vmult(v, (STAGNANT_VELOCITY * algaeBeSlow) / mag);
  }
}

void motor_updateAcceleration(Motor *m) {}

void motor_updateAccelerationAlgae(Motor *m) {
  Vector2 accelerationStep = m->acceleration;
  accelerationStep = pond_vmult(accelerationStep, -1.0f);
  accelerationStep = pond_vdiv(accelerationStep, powf(m->self->mass, 3));
  Vector2 acceleration =
      pond_vmult(m->acceleration, ACCELERATION_GROWTH_FACTOR * 0.1f);
  acceleration = pond_vadd(acceleration, accelerationStep);
  m->acceleration = acceleration;
}

float motor_clamp(Motor *m, float input, float lowerLimit, float upperLimit) {
  if (input >= upperLimit) {
    return upperLimit;
  }
  if (input <= lowerLimit) {
    return lowerLimit;
  }
  return input;
}

void motor_clampToWall(Motor *m) {
  float x = m->displacement.x;
  float y = m->displacement.y;
  x = motor_clamp(m, x, 0, g_width);
  y = motor_clamp(m, y, 0, g_height);
  m->displacement.x = x;
  m->displacement.y = y;
}

void motor_bounceIfWallCollision(Motor *m) {
  if (m->displacement.x >= g_width || m->displacement.x <= 0) {
    m->velocity.x = m->velocity.x * COLLISION_DAMPING_FACTOR;
    m->acceleration.x = m->acceleration.x * COLLISION_DAMPING_FACTOR;
    return;
  }

  if (m->displacement.y >= g_height || m->displacement.y <= 0) {
    m->velocity.y = m->velocity.y * COLLISION_DAMPING_FACTOR;
    m->acceleration.y = m->acceleration.y * COLLISION_DAMPING_FACTOR;
  }
}

void motor_bounceIfWallCollisionSnake(Motor *m) {
  if (m->displacement.x >= g_width || m->displacement.x <= 0) {
    m->velocityPropagation.x = m->velocity.x * COLLISION_DAMPING_FACTOR;
    m->acceleration.x = m->acceleration.x * COLLISION_DAMPING_FACTOR;
    return;
  }

  if (m->displacement.y >= g_height || m->displacement.y <= 0) {
    m->velocityPropagation.y = m->velocity.y * COLLISION_DAMPING_FACTOR;
    m->acceleration.y = m->acceleration.y * COLLISION_DAMPING_FACTOR;
  }
}

void motor_bounceIfOrganismCollision(Motor *m) {
  if (m->self == NULL) {
    return;
  }
  if (m->self->collidingCount == 0) {
    return;
  }

  for (int i = 0; i < m->self->collidingCount; i++) {
    Organism *other = m->self->collidingWith[i];
    motor_moveOutOfOverlap(m, other);
    motor_bounceAgainstOther(m, other);

    int n = other->collidingCount;
    for (int j = 0; j < n; j++) {
      if (other->collidingWith[j] == m->self) {
        other->collidingWith[j] = other->collidingWith[n - 1];
        n--;
        break;
      }
    }
    other->collidingCount = n;
  }

  m->self->collidingCount = 0;
}

void motor_moveOutOfOverlap(Motor *m, Organism *other) {
  Vector2 selfToOther = pond_vsub(other->motor->displacement, m->displacement);
  Vector2 xUnitVector = pond_vnormalize(selfToOther);

  Vector2 selfRightEdgePoint =
      pond_vadd(m->displacement, pond_vmult(xUnitVector, m->self->mass / 2.0f));
  Vector2 otherLeftEdgePoint = pond_vsub(
      other->motor->displacement, pond_vmult(xUnitVector, other->mass / 2.0f));
  Vector2 otherLeftEdgeToSelfRightEdge =
      pond_vsub(selfRightEdgePoint, otherLeftEdgePoint);

  float otherWeight = powf(m->self->mass, 3);
  float selfWeight = powf(other->mass, 3);
  float totalWeight = selfWeight + otherWeight;
  selfWeight /= totalWeight;
  otherWeight /= totalWeight;

  Vector2 newSelfDisplacement = pond_vsub(
      m->displacement, pond_vmult(otherLeftEdgeToSelfRightEdge, selfWeight));
  m->displacement = newSelfDisplacement;
  Vector2 newOtherDisplacement =
      pond_vadd(other->motor->displacement,
                pond_vmult(otherLeftEdgeToSelfRightEdge, otherWeight));
  other->motor->displacement = newOtherDisplacement;
}

void motor_bounceAgainstOther(Motor *m, Organism *other) {
  Vector2 otherToSelfVector =
      pond_vsub(m->displacement, other->motor->displacement);
  Vector2 otherToSelfUnitVector = pond_vnormalize(otherToSelfVector);

  painter_debugCollisions(m->self, other);

  Vector2 newVelocity = otherToSelfUnitVector;
  newVelocity = pond_vmult(newVelocity, (other->mass * 2.0f) /
                                            (other->mass + m->self->mass));
  newVelocity = pond_vmult(
      newVelocity, pond_vdot(otherToSelfUnitVector,
                             pond_vsub(m->velocity, other->motor->velocity)));
  Vector2 newMVelocity = pond_vsub(m->velocity, newVelocity);
  m->velocity = newMVelocity;

  Vector2 selfToOtherUnitVector = pond_vmult(otherToSelfUnitVector, -1.0f);
  Vector2 newOtherVelocity =
      pond_vmult(selfToOtherUnitVector,
                 (m->self->mass * 2.0f) / (other->mass + m->self->mass));
  newOtherVelocity =
      pond_vmult(newOtherVelocity,
                 pond_vdot(selfToOtherUnitVector,
                           pond_vsub(other->motor->velocity, m->velocity)));
  other->motor->velocity = pond_vsub(other->motor->velocity, newOtherVelocity);

  if (m->kind == MOTOR_KIND_ALGAE && other->genome.species == SPECIES_ALGAE) {
    motor_agglutinate(m, other);
  }
}

void motor_agglutinate(Motor *m, Organism *other) {
  Vector2 velocityCopy = m->velocity;
  Vector2 accelerationCopy = m->acceleration;
  Vector2 newVelocity = pond_vadd(m->velocity, other->motor->velocity);
  m->velocity = pond_vmult(newVelocity, 0.5f);
  Vector2 newAcceleration =
      pond_vadd(m->acceleration, other->motor->acceleration);
  m->acceleration = pond_vmult(newAcceleration, 0.5f);
  Vector2 newOtherVelocity = pond_vadd(other->motor->velocity, velocityCopy);
  newOtherVelocity = pond_vmult(newOtherVelocity, 0.5f);
  other->motor->velocity = newOtherVelocity;

  Vector2 newOtherAcceleration =
      pond_vadd(other->motor->acceleration, accelerationCopy);
  other->motor->acceleration = pond_vmult(newOtherAcceleration, 0.5f);

  painter_debugCollisions(m->self, other);
}
