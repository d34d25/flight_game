#pragma once

#include "helpers.h"

constexpr float MASS = 1.0f;

constexpr float INERTIA = 0.1667f;

struct Body
{
    Transform transform = {};

    Vector3 force = {0.0f,0.0f,0.0f};
    Vector3 torque = {0.0f,0.0f,0.0f};

    Vector3 linearVelocity = {0.0f,0.0f,0.0f};
    Vector3 angularVelocity = {0.0f,0.0f,0.0f};

    Vector3 angularDrag = {0.0f,0.0f,0.0f};

    float forwardDrag = 0.0f;
    float sideDrag = 0.0f;
};

inline void ApplyForce(Body& body, const Vector3& direction, float amount)
{
    body.force += direction * amount;
}

inline void ApplyTorque(Body& body, const Vector3& direction, float amount)
{
    body.torque += direction * amount;
}

inline void ApplyForceLocal(Body& body, const Vector3& direction, float amount)
{
    Vector3 bodyDirection = GetWorldVectorFromLocalVector(body.transform.rotation, direction);

    body.force += bodyDirection * amount;
}

inline void ApplyTorqueLocal(Body& body, const Vector3& direction, float amount)
{
    Vector3 bodyDirection = GetWorldVectorFromLocalVector(body.transform.rotation, direction);

    body.torque += bodyDirection * amount;
}

void UpdateBody(Body& body, float dt);