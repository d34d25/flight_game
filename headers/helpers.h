#pragma once

#include "raylib.h"
#include "raymath.h"

#include <vector>

constexpr Vector3 LOCAL_FORWARD = {0.0f,0.0f,1.0f};

constexpr Vector3 LOCAL_UP = {0.0f,1.0f,0.0f};

constexpr Vector3 LOCAL_RIGHT = {1.0f,0.0f,0.0f};

constexpr Vector3 LOCAL_BACKWARD = {0.0f,0.0f, -1.0f};

constexpr Vector3 LOCAL_DOWN = {0.0f,-1.0f,0.0f};

constexpr Color TRANSPARENT_BLUE = {150, 150, 255, 255};

constexpr Color DEEP_BLUE = {25,25,100,255};

constexpr Color VERY_LIGHT_GRAY = {235,235,235,255};

constexpr float WORLD_SIZE = 10000;

//makes transformA follow transformB, an offset can be added
inline void FollowTransform(Transform& transformA, const Transform& transformB, const Vector3& offset)
{
    Vector3 rotatedOffset = Vector3RotateByQuaternion(offset, transformB.rotation);

    transformA.translation = transformB.translation + rotatedOffset;

    transformA.rotation = transformB.rotation;
}

//transform a local space vector into a world space vector, the returned vector is already normalized
inline Vector3 GetWorldVectorFromLocalVector(const Quaternion& rotation, const Vector3& direction)
{
    Vector3 localVec = Vector3RotateByQuaternion(direction, QuaternionNormalize(rotation));

    localVec = Vector3Normalize(localVec);

    return localVec;
}

inline float GetDesiredValue(float desiredValue, float drag)
{
    return desiredValue * drag;
}

//projects a X and Z 3D positions into 2D X and Y position
inline Vector2 _3DXZTo2DXY(Vector2 xz, float worldSize, int imgWidth, int imgHeight)
{
    Vector2 xzvec = xz;

    xzvec /= worldSize;

    xzvec.x *= imgWidth;
    xzvec.y *= imgHeight;

    return xzvec;
}

//transforms a set of local vertices based on a given transform
inline std::vector<Vector3> GetTransformedVertices(const std::vector<Vector3>& vertices, const Transform& transform)
{
    std::vector<Vector3> transformedVertices = {};

    for(const Vector3& v : vertices)
    {
        Vector3 vertex = v;

        vertex = Vector3RotateByQuaternion(vertex, transform.rotation);

        vertex = Vector3Add(vertex, transform.translation);

        transformedVertices.push_back(vertex);
    }

    return transformedVertices;
}