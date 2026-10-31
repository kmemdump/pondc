#include "pond/motor.h"
#include "pond/log.h"
#include "pond/types.h"
#include "pond/organism.h"
#include <math.h>
#include <stdlib.h>

Motor *motor_create(MotorKind kind, Organism *organism, Vector2 startDisplacement, Vector2 startVelocity)
{
    Motor *m = (Motor *)calloc(1, sizeof(Motor));
    m->self = organism;
    m->displacement = startDisplacement;
    m->velocity = startVelocity;
    m->acceleration = pond_vcopy((Vector2){0.0f, 0.0f});
    m->timeOffset = pond_random(0.0f, CLOCK_MAX);
    m->kind = kind;
    m->velocityPropagation = pond_vcopy(startVelocity);
    m->velocityNormal = pond_vcopy(startVelocity);
    m->algaeBeSlow = 0.2f;
    return m;
}

void motor_update(Motor *m)
{
    if (m->self == NULL)
    {
        pond_log_error("Motor has no self.");
        return;
    }

    motor_bounceIfOrganismCollision(m);

    if (m->kind == MOTOR_KIND_SNAKE)
    {
        motor_bounceIfWallCollisionSnake(m);
    }
    else
    {
        motor_bounceIfWallCollision(m);
    }

    motor_move(m);
    motor_clampToWall(m);
}

void motor_move(Motor *m)
{
    motor_updateAcceleration(m);
    motor_updateVelocity(m);
    motor_updateDisplacement(m);
}

void motor_updateDisplacement(Motor *m)
{
    Vector2 d = organism_displacement(m->self);
    d = pond_vadd(d, m->velocity);
    organism_setDisplacement(m->self, d);
}

void motor_updateVelocity(Motor *m)
{
    Vector2 v = organism_velocity(m->self);
    v = pond_vadd(v, organism_acceleration(m->self));
    float mag = pond_vmag(v);
    if (mag > TERMINAL_VELOCITY)
    {
        v = pond_vdiv(v, mag / TERMINAL_VELOCITY);
        organism_setVelocity(m->self, v);
    }
    else if (mag < STAGNANT_VELOCITY)
    {
        v = pond_vmult(v, STAGNANT_VELOCITY / mag);
        organism_setVelocity(m->self, v);
    }

    if (m->kind == MOTOR_KIND_SNAKE)
    {
        motor_updateVelocitySnake(m);
    }
    else if (m->kind == MOTOR_KIND_ALGAE)
    {
        motor_updateVelocityAlgae(m);
    }
}

void motor_updateVelocitySnake(Motor *m)
{
    Vector2 velocityPropagation = organism_velocity(m->self);
    velocityPropagation = pond_vadd(velocityPropagation, organism_acceleration(m->self));
    m->velocityPropagation = velocityPropagation;

    Vector2 rotated = (Vector2){-m->velocityNormal.y, m->velocityNormal.x};
    m->velocityNormal = pond_vmult(rotated, sinf(g_clock + m->timeOffset));
    m->velocityNormal = pond_vmult(m->velocityNormal, SLITHER_FACTOR);
    Vector2 v = pond_vadd(m->velocityPropagation, m->velocityNormal);
    m->velocity = v;

    float mag = pond_vmag(m->velocity);
    if (mag > TERMINAL_VELOCITY * 2)
    {
        m->velocity = pond_vdiv(m->velocity, mag / (TERMINAL_VELOCITY * 2));
    }
    else if (mag < STAGNANT_VELOCITY * 2)
    {
        m->velocity = pond_vmult(m->velocity, (STAGNANT_VELOCITY * 2) / mag);
    }
}

void motor_updateVelocityAlgae(Motor *m)
{
    Vector2 v = organism_velocity(m->self);
    v = pond_vadd(v, organism_acceleration(m->self));

    float mag = pond_vmag(v);
    if (mag == 0.0f)
    {
        return;
    }

    float algaeBeSlow = 0.2f;

    if (mag > TERMINAL_VELOCITY * algaeBeSlow)
    {
        v = pond_vdiv(v, mag / (TERMINAL_VELOCITY * algaeBeSlow));
        organism_setVelocity(m->self, v);
    }
    else if (mag < STAGNANT_VELOCITY * algaeBeSlow)
    {
        v = pond_vmult(v, (STAGNANT_VELOCITY * algaeBeSlow) / mag);
        organism_setVelocity(m->self, v);
    }
}

void motor_updateAcceleration(Motor *m)
{
    Vector2 accelerationStep = pond_vcopy((Vector2){0.0f, 0.0f});
    accelerationStep = pond_vadd(accelerationStep, pond_vmult(pond_vrandom2D(), ACCELERATION_NOISE_FACTOR * 12.0f * sinf(g_clock + m->timeOffset)));
    accelerationStep = pond_vdiv(accelerationStep, powf(motor_mass(m), 3));
    Vector2 acceleration = pond_vmult(m->acceleration, ACCELERATION_GROWTH_FACTOR * 0.5f);
    acceleration = pond_vadd(acceleration, accelerationStep);
    organism_setAcceleration(m->self, acceleration);

    if (m->kind == MOTOR_KIND_ALGAE)
    {
        motor_updateAccelerationAlgae(m);
    }
}

void motor_updateAccelerationAlgae(Motor *m)
{
    Vector2 accelerationStep = pond_vcopy(organism_acceleration(m->self));
    accelerationStep = pond_vmult(accelerationStep, -1.0f);
    accelerationStep = pond_vdiv(accelerationStep, powf(motor_mass(m), 3));
    Vector2 acceleration = pond_vmult(m->acceleration, ACCELERATION_GROWTH_FACTOR * 0.1f);
    acceleration = pond_vadd(acceleration, accelerationStep);
    organism_setAcceleration(m->self, acceleration);
}

float motor_clamp(Motor *m, float input, float lowerLimit, float upperLimit)
{
    if (input >= upperLimit) { return upperLimit; }
    if (input <= lowerLimit) { return lowerLimit; }
    return input;
}

void motor_clampToWall(Motor *m)
{
    float x = m->displacement.x;
    float y = m->displacement.y;
    x = motor_clamp(m, x, 0, g_width);
    y = motor_clamp(m, y, 0, g_height);
    m->displacement.x = x;
    m->displacement.y = y;
}

void motor_bounceIfWallCollision(Motor *m)
{
    if (m->displacement.x >= g_width || m->displacement.x <= 0)
    {
        m->velocity.x = m->velocity.x * COLLISION_DAMPING_FACTOR;
        m->acceleration.x = m->acceleration.x * COLLISION_DAMPING_FACTOR;
        return;
    }

    if (m->displacement.y >= g_height || m->displacement.y <= 0)
    {
        m->velocity.y = m->velocity.y * COLLISION_DAMPING_FACTOR;
        m->acceleration.y = m->acceleration.y * COLLISION_DAMPING_FACTOR;
    }
}

void motor_bounceIfWallCollisionSnake(Motor *m)
{
    if (m->displacement.x >= g_width || m->displacement.x <= 0)
    {
        m->velocityPropagation.x = m->velocity.x * COLLISION_DAMPING_FACTOR;
        m->acceleration.x = m->acceleration.x * COLLISION_DAMPING_FACTOR;
        return;
    }

    if (m->displacement.y >= g_height || m->displacement.y <= 0)
    {
        m->velocityPropagation.y = m->velocity.y * COLLISION_DAMPING_FACTOR;
        m->acceleration.y = m->acceleration.y * COLLISION_DAMPING_FACTOR;
    }
}

void motor_bounceIfOrganismCollision(Motor *m)
{
    if (m->self == NULL) { return; }
    if (m->self->collidingCount == 0) { return; }

    for (int i = 0; i < m->self->collidingCount; i++)
    {
        Organism *other = m->self->collidingWith[i];
        motor_moveOutOfOverlap(m, other);
        motor_bounceAgainstOther(m, other);

        int n = other->collidingCount;
        for (int j = 0; j < n; j++)
        {
            if (other->collidingWith[j] == m->self)
            {
                other->collidingWith[j] = other->collidingWith[n - 1];
                n--;
                break;
            }
        }
        other->collidingCount = n;
    }

    m->self->collidingCount = 0;
}

void motor_moveOutOfOverlap(Motor *m, Organism *other)
{
    Vector2 selfToOther = pond_vsub(organism_displacement(other), organism_displacement(m->self));
    Vector2 xUnitVector = pond_vnormalize(selfToOther);

    Vector2 selfRightEdgePoint = pond_vadd(organism_displacement(m->self), pond_vmult(xUnitVector, m->self->mass / 2.0f));
    Vector2 otherLeftEdgePoint = pond_vsub(organism_displacement(other), pond_vmult(xUnitVector, other->mass / 2.0f));
    Vector2 otherLeftEdgeToSelfRightEdge = pond_vsub(selfRightEdgePoint, otherLeftEdgePoint);

    float otherWeight = powf(m->self->mass, 3);
    float selfWeight = powf(other->mass, 3);
    float totalWeight = selfWeight + otherWeight;
    selfWeight /= totalWeight;
    otherWeight /= totalWeight;

    Vector2 newSelfDisplacement = pond_vsub(organism_displacement(m->self), pond_vmult(otherLeftEdgeToSelfRightEdge, selfWeight));
    organism_setDisplacement(m->self, newSelfDisplacement);
    Vector2 newOtherDisplacement = pond_vadd(organism_displacement(other), pond_vmult(otherLeftEdgeToSelfRightEdge, otherWeight));
    organism_setDisplacement(other, newOtherDisplacement);
}

void motor_bounceAgainstOther(Motor *m, Organism *other)
{
    Vector2 otherToSelfVector = pond_vsub(organism_displacement(m->self), organism_displacement(other));
    Vector2 otherToSelfUnitVector = pond_vnormalize(otherToSelfVector);

    if (m->self->painter && m->self->painter->drawCollision)
    {
        // debug collision lines
    }

    Vector2 newVelocity = pond_vcopy(otherToSelfUnitVector);
    newVelocity = pond_vmult(newVelocity, (other->mass * 2.0f) / (other->mass + motor_mass(m)));
    newVelocity = pond_vmult(newVelocity, pond_vdot(otherToSelfUnitVector, pond_vsub(organism_velocity(m->self), organism_velocity(other))));
    Vector2 newMVelocity = pond_vsub(organism_velocity(m->self), newVelocity);
    organism_setVelocity(m->self, newMVelocity);

    Vector2 selfToOtherUnitVector = pond_vmult(otherToSelfUnitVector, -1.0f);
    Vector2 newOtherVelocity = pond_vmult(selfToOtherUnitVector, (motor_mass(m) * 2.0f) / (other->mass + motor_mass(m)));
    newOtherVelocity = pond_vmult(newOtherVelocity, pond_vdot(selfToOtherUnitVector, pond_vsub(organism_velocity(other), organism_velocity(m->self))));
    Vector2 newOVelocity = pond_vsub(organism_velocity(other), newOtherVelocity);
    organism_setVelocity(other, newOVelocity);

    if (m->kind == MOTOR_KIND_ALGAE && other->dna.species == SPECIES_ALGAE)
    {
        motor_agglutinate(m, other);
    }
}

void motor_agglutinate(Motor *m, Organism *other)
{
    Vector2 velocityCopy = organism_velocity(m->self);
    Vector2 accelerationCopy = organism_acceleration(m->self);

    Vector2 newVelocity = pond_vadd(organism_velocity(m->self), organism_velocity(other));
    newVelocity = pond_vmult(newVelocity, 0.5f);
    organism_setVelocity(m->self, newVelocity);

    Vector2 newAcceleration = pond_vadd(organism_acceleration(m->self), organism_acceleration(other));
    newAcceleration = pond_vmult(newAcceleration, 0.5f);
    organism_setAcceleration(m->self, newAcceleration);

    Vector2 newOtherVelocity = pond_vadd(organism_velocity(other), velocityCopy);
    newOtherVelocity = pond_vmult(newOtherVelocity, 0.5f);
    organism_setVelocity(other, newOtherVelocity);

    Vector2 newOtherAcceleration = pond_vadd(organism_acceleration(other), accelerationCopy);
    newOtherAcceleration = pond_vmult(newOtherAcceleration, 0.5f);
    organism_setAcceleration(other, newOtherAcceleration);

    if (m->self->painter && m->self->painter->drawCollision)
    {
        // debug collision lines
    }
}

float motor_mass(Motor *m)
{
    return m->self ? m->self->mass : 0.0f;
}
