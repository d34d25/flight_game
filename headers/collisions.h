#pragma once

#include "helpers.h"

#include <vector>

#include "raymath.h"

struct Projection
{
    float min;
    float max;
};

struct Collider
{
    std::vector<Vector3> localVertices;

    std::vector<std::pair<int, int>> edgesIndexes;

    std::vector<std::vector<int>> facesIndexes;
};

Collider CreatePrismatoid(
    float baseWidth,
    float baseLength,
    float topWidth,
    float topLength,
    float height,
    Quaternion rotation = QuaternionIdentity(),
    Vector3 offset = {0.0f,0.0f,0.0f}
);

inline bool CollidedWithTerrain(Vector3 position, float worldSize, float ySize ,const Image& masterImg)
{
    Vector2 _2dPos = _3DXZTo2DXY({position.x, position.z}, worldSize, masterImg.width, masterImg.height);

    if(_2dPos.x < 0.0f || _2dPos.x >= masterImg.width || 
    _2dPos.y < 0.0f || _2dPos.y >= masterImg.height)
    {
        return false;    
    }

    Color pixelColor = GetImageColor(masterImg, (int)_2dPos.x, (int)_2dPos.y);

    float heightFromTerrain = (pixelColor.r / 255.0f) * ySize;

    if(position.y <= heightFromTerrain) return true;
   
    return false;
}

inline Projection ProjectVertices3D(const std::vector<Vector3> &vertices, const Vector3 axis)
{
    Projection projection = {};

    projection.min = INFINITY;
    projection.max = -INFINITY;

    for(const Vector3& v : vertices)
    {
        float dot = Vector3DotProduct(v, axis);

        if(dot < projection.min) projection.min = dot;

        if(dot > projection.max) projection.max = dot;
    }

    return projection;
}

bool SAT3DCCD(
    const Collider& colliderA, const Transform& transformA, const Vector3 velocityA,
    const Collider& colliderB, const Transform& transformB, const Vector3 velocityB,
    float dt
);