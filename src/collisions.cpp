#include "collisions.h"

const float EPSILON_SQR = EPSILON * EPSILON;

Collider CreatePrismatoid(float baseWidth, float baseLength, float topWidth, float topLength, float height, Quaternion rotation, Vector3 offset)
{
    Collider collider = {};

    float halfBaseW = baseWidth * 0.5f;
    float halfBaseL = baseLength * 0.5f;

    float halfTopW = topWidth * 0.5f;
    float halfTopL = topLength * 0.5f;

    float halfHeight = height * 0.5f;

    std::vector<Vector3> vertices = {

        //bottom face

        {-halfBaseW, -halfHeight, -halfBaseL}, //0
        {halfBaseW, -halfHeight, -halfBaseL}, //1
        {halfBaseW, -halfHeight, halfBaseL}, //2
        {-halfBaseW, -halfHeight, halfBaseL}, //3

        //top face

        {-halfTopW, halfHeight, -halfTopL}, //4
        {halfTopW, halfHeight, -halfTopL}, //5
        {halfTopW, halfHeight, halfTopL}, //6
        {-halfTopW, halfHeight, halfTopL} //7
    };

    for(Vector3& v : vertices)
    {
        v = Vector3RotateByQuaternion(v, rotation);

        v = Vector3Add(v, offset);
    }

    collider.localVertices = vertices;

    //faces

    collider.facesIndexes.push_back({0, 1, 2, 3});

    collider.facesIndexes.push_back({4, 7, 6, 5});

    //side faces
    collider.facesIndexes.push_back({0, 4, 5, 1}); //back side
    collider.facesIndexes.push_back({1, 5, 6, 2}); //right side
    collider.facesIndexes.push_back({2, 6, 7, 3}); //front side
    collider.facesIndexes.push_back({3, 7, 4, 0}); //left side

    //edges

    collider.edgesIndexes.push_back({0, 1});
    collider.edgesIndexes.push_back({1, 2});
    collider.edgesIndexes.push_back({2, 3});
    collider.edgesIndexes.push_back({3, 0});
    collider.edgesIndexes.push_back({4, 5});
    collider.edgesIndexes.push_back({5, 6});
    collider.edgesIndexes.push_back({6, 7});
    collider.edgesIndexes.push_back({7, 4});
    collider.edgesIndexes.push_back({0, 4});
    collider.edgesIndexes.push_back({1, 5});
    collider.edgesIndexes.push_back({2, 6});
    collider.edgesIndexes.push_back({3, 7});

    return collider;
}

bool SAT3DCCD(const Collider &colliderA, const Transform &transformA, const Vector3 velocityA, const Collider &colliderB, const Transform &transformB, const Vector3 velocityB, float dt)
{
    std::vector<Vector3> axes;

    std::vector<Vector3> verticesA = GetTransformedVertices(colliderA.localVertices, transformA);

    std::vector<Vector3> verticesB = GetTransformedVertices(colliderB.localVertices, transformB);

    if(verticesA.empty() || verticesB.empty()) return false;

    // collider A

    //faces

    for(const std::vector<int>& face : colliderA.facesIndexes)
    {
        if(face.size() < 3) continue;

        Vector3 v0 = verticesA[face[0]];
        Vector3 v1 = verticesA[face[1]];
        Vector3 v2 = verticesA[face[2]];

        Vector3 edge1 = v1 - v0;
        Vector3 edge2 = v2 - v0;

        Vector3 normal = Vector3CrossProduct(edge1, edge2);

        if(Vector3LengthSqr(normal) > EPSILON_SQR)
        {
            axes.push_back(Vector3Normalize(normal));
        }
    }

    //collider b

    //faces

    for(const std::vector<int>& face : colliderB.facesIndexes)
    {
        if(face.size() < 3) continue;

        Vector3 v0 = verticesB[face[0]];
        Vector3 v1 = verticesB[face[1]];
        Vector3 v2 = verticesB[face[2]];

        Vector3 edge1 = v1 - v0;
        Vector3 edge2 = v2 - v0;

        Vector3 normal = Vector3CrossProduct(edge1, edge2);

        if(Vector3LengthSqr(normal) > EPSILON_SQR)
        {
            axes.push_back(Vector3Normalize(normal));
        }
    }

    //edges

    for(const std::pair<int, int>& edgeA : colliderA.edgesIndexes)
    {
        Vector3 edgeAP1 = verticesA[edgeA.first];
        Vector3 edgeAP2 = verticesA[edgeA.second];

        Vector3 edgeAVec = edgeAP2 - edgeAP1;

        for(const std::pair<int, int>& edgeB : colliderB.edgesIndexes)
        {
            Vector3 edgeBP1 = verticesB[edgeB.first];
            Vector3 edgeBP2 = verticesB[edgeB.second];

            Vector3 edgeBVec = edgeBP2 - edgeBP1;

            Vector3 axis = Vector3CrossProduct(edgeAVec, edgeBVec);

            if(Vector3LengthSqr(axis) > EPSILON_SQR)
            {
                axes.push_back(Vector3Normalize(axis));
            }
        }
    }

    //CCD

    double frameStart = 0.0;
    double frameEnd = 1.0;

    for(const Vector3& axis : axes)
    {
        Projection projA = ProjectVertices3D(verticesA, axis);
        Projection projB = ProjectVertices3D(verticesB, axis);

        double relativeSpeed = Vector3DotProduct((velocityB - velocityA) * dt, axis);

        if(fabs(relativeSpeed) > EPSILON)
        {
            double tEnter = (projA.min - projB.max) / relativeSpeed;
            double tExit = (projA.max - projB.min) / relativeSpeed;

            if(tEnter > tExit) std::swap(tEnter, tExit);

            tEnter = std::max(tEnter, 0.0);
            tExit = std::min(tExit, 1.0);

            if(tEnter > tExit) return false;

            frameStart = std::max(frameStart, tEnter);
            frameEnd = std::min(frameEnd, tExit);

            if(frameStart > frameEnd) return false;
        }
        else
        {
            if(projA.max < projB.min || projB.max < projA.min) return false;
        }
    }

    if(axes.empty()) return false;

    return frameStart < 1.0 && frameEnd > 0.0 && frameStart <= frameEnd && frameEnd >= frameStart;
}
