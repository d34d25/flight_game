#pragma once

#include "raylib.h"
#include "raymath.h"

constexpr Vector3 LOCAL_FORWARD = {0.0f,0.0f,1.0f};

constexpr Vector3 LOCAL_UP = {0.0f,1.0f,0.0f};

constexpr Vector3 LOCAL_RIGHT = {1.0f,0.0f,0.0f};

constexpr Vector3 LOCAL_BACKWARD = {0.0f,0.0f, -1.0f};

constexpr Vector3 LOCAL_DOWN = {0.0f,-1.0f,0.0f};

inline void FollowTransform(Transform& transformA, const Transform& transformB, const Vector3& offset)
{
    Vector3 rotatedOffset = Vector3RotateByQuaternion(offset, transformB.rotation);

    transformA.translation = transformB.translation + rotatedOffset;

    transformA.rotation = transformB.rotation;
}

//transform a local space vector into a world space vector 
inline Vector3 GetWorldVectorFromLocalVector(const Quaternion& rotation, const Vector3& direction)
{
    Vector3 localVec = Vector3RotateByQuaternion(direction, QuaternionNormalize(rotation));

    localVec = Vector3Normalize(localVec);

    return localVec;
}