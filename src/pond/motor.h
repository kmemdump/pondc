#ifndef POND_MOTOR_H
#define POND_MOTOR_H

#include "pond/types.h"

typedef enum MotorKind {
  MOTOR_KIND_BASE = 0,
  MOTOR_KIND_SNAKE,
  MOTOR_KIND_FISH,
  MOTOR_KIND_ALGAE
} MotorKind;

typedef struct Motor Motor;
struct Motor {
  MotorKind kind;
  Organism *self;
  Vector2 displacement;
  Vector2 velocity;
  Vector2 acceleration;
  Vector2 velocityPropagation;
  Vector2 velocityNormal;
  float timeOffset;
  float algaeBeSlow;
};

Motor *motor_create(MotorKind kind, Organism *organism,
                    Vector2 startDisplacement, Vector2 startVelocity);
void motor_update(Motor *m);
void motor_move(Motor *m);
float motor_clamp(Motor *m, float input, float lowerLimit, float upperLimit);
void motor_clampToWall(Motor *m);
void motor_bounceIfWallCollision(Motor *m);
void motor_bounceIfOrganismCollision(Motor *m);
void motor_moveOutOfOverlap(Motor *m, Organism *other);
void motor_bounceAgainstOther(Motor *m, Organism *other);
void motor_updateVelocitySnake(Motor *m);
void motor_updateVelocityAlgae(Motor *m);
void motor_updateAccelerationAlgae(Motor *m);
void motor_bounceIfWallCollisionSnake(Motor *m);
void motor_agglutinate(Motor *m, Organism *other);

#endif
