#pragma once

#include <iostream>

#include "player.h"

#include "rlgl.h"

#include <GL/gl.h>

#include "collisions.h"

#include "enemy.h"

constexpr float NATIVE_WIDTH = 1600;
constexpr float NATIVE_HEIGHT = 900;

constexpr float CANVAS_WIDTH = 256;//256;
constexpr float CANVAS_HEIGHT = 144;//144;

constexpr int SCALE = 4;

inline Camera3D renderCamera;

struct Scene
{
    Player player;

    Enemy testEnemyFighter;

    //Terrain terrain;

    Model floor;

    Model skySphereModel;

    RenderTexture backgroundCanvas;

    RenderTexture terrainCanvas;

    RenderTexture gameplayCanvas;

    Shader skyShader;

    Shader flatShader;

    Shader fogScrollShader;

    //sky shader
    Color baseColor;
    Color topColor;
    Color bottomColor;
    //------------------

    //fog shader
    Vector3 viewPos;
    //------------------

    Vector3 lightDir;

    float minHeight;
    float maxHeight;

    float baseMinHeight;
    float baseMaxHeight;

    float skyRadius;
    //------------------

    //fog shader
    float fogDensity;

    float fogStart;
    float fogEnd;
    //------------------

    //flat shader
    float minIntensity;
    float maxIntensity;

    int lightDirLoc;
    int minIntensityLoc;
    int maxIntensityLoc;
    //------------------

    //sky shader
    int baseColorLoc;
    int topColorLoc;
    int bottomColorLoc;
    int minHeightLoc;
    int maxHeightLoc;
    //------------------

    //fog shader
    int fogColorLoc;
    int fogDensityLoc;
    int viewPosLoc;
    int fogStartLoc;
    int fogEndLoc;
    int playerXZLoc;
    int scrollingLoc;
    //------------------

    //debug
    bool playerCollidedTerrain = false;
    bool playerCollidedSATCCD = false;

    Transform obstacleTransform;
    Collider obstacleCollider;
};

void InitScene(Scene& scene);

void UpdateScene(Scene& scene, float dt);

void DrawGameplay(Scene& scene);

void DrawScene(Scene& scene);

void UnloadScene(Scene& scene);

inline void DrawSky(Scene& scene)
{
    rlPushMatrix();

    rlDisableBackfaceCulling();
    rlDisableDepthMask();

    Matrix originalView = rlGetMatrixModelview();

    Matrix view = GetCameraMatrix(camera);

    view.m12 = 0.0f; view.m13 = 0.0f; view.m14 = 0.0f;

    rlSetMatrixModelview(view);

    DrawModel(scene.skySphereModel, {0,0,0}, 3, WHITE);

    rlEnableDepthMask();
    rlEnableBackfaceCulling();

    rlSetMatrixModelview(originalView);

    rlPopMatrix();
}

inline void DrawPlayer(Aircraft& aircraft)
{
    Model& model = GetAircraftModel(aircraft);
    
    Vector3 rotationAxis = {0.0f,0.0f,0.0f};
    
    float rotationAngle = 0.0f;

    QuaternionToAxisAngle(aircraft.body.transform.rotation, &rotationAxis, &rotationAngle);

    DrawModelEx(model, {0,0,0}, rotationAxis, rotationAngle * RAD2DEG, {1.0f,1.0f,1.0f}, aircraft.debugColor);
}

inline void DrawAircraft(Aircraft& aircraft, Vector3 playerPos)
{
    Model& model = GetAircraftModel(aircraft);
    
    Vector3 rotationAxis = {0.0f,0.0f,0.0f};
    
    float rotationAngle = 0.0f;

    Vector3 aircraftPos = aircraft.body.transform.translation - playerPos;

    QuaternionToAxisAngle(aircraft.body.transform.rotation, &rotationAxis, &rotationAngle);

    float scale = 1.0f; //scale for debug

    DrawModelEx(model, aircraftPos, rotationAxis, rotationAngle * RAD2DEG, {scale,scale,scale}, WHITE);
}

inline void DrawBullets(Aircraft& aircraft, Vector3 playerPos)
{
    for(Bullet* b : aircraft.bulletpool.activeBullets)
    {
        if(!b) continue;

        rlPushMatrix();

        rlTranslatef(b->transform.translation.x - playerPos.x, b->transform.translation.y - playerPos.y, b->transform.translation.z - playerPos.z);

        rlMultMatrixf(MatrixToFloat(QuaternionToMatrix(b->transform.rotation)));

        DrawCubeV(
            {0.0f,0.0f,0.0f},
            {b->properties.bulletSize * 0.5f, b->properties.bulletSize * 0.5f,b->properties.bulletSize},
            YELLOW
        );

        rlPopMatrix();
    }
}

inline void DrawBG()
{
    //replace with plane models for the background

    //create a fog shader that combines the z and y axis for it (fade out towards y = 0 + normal fog)

    //use pre defined positions for the meshes

    //create an enum that dictates the floor and sky colors and the background mesh variant

    float yOffset = -100;

    float zOffset = 200;

    float size = 100;

    Vector3 v1 = {0,0 + yOffset,zOffset};

    Vector3 v2 = {0,size + yOffset,zOffset};

    Vector3 v3 = {size, 0 + yOffset, zOffset};

    DrawTriangle3D(v1,v2,v3, DARKGREEN);
}

inline void DrawMissiles(Aircraft& aircraft, Vector3 playerPos)
{
    for(Missile* m : aircraft.missilePool.activeMissiles)
    {
        if(!m) continue;

        rlPushMatrix();

        rlTranslatef(m->body.transform.translation.x - playerPos.x, m->body.transform.translation.y - playerPos.y, m->body.transform.translation.z - playerPos.z);

        rlMultMatrixf(MatrixToFloat(QuaternionToMatrix(m->body.transform.rotation)));

        DrawCubeV(
            {0.0f,0.0f,0.0f},
            {m->properties.missileSize * 0.5f, m->properties.missileSize * 0.5f,m->properties.missileSize},
            RAYWHITE
        );

        rlPopMatrix();

        for(Trail* t : m->trailPool.activeTrails)
        {
            if(!t) continue;

            Color trailcolor = WHITE;

            trailcolor.a = (unsigned char)t->alpha;

            rlPushMatrix();

            Matrix cameraMatrix = GetCameraMatrix(renderCamera);

            Matrix lookMatrix = MatrixLookAt(t->position - playerPos, renderCamera.position, LOCAL_UP);

            Matrix billboardMatrix = MatrixInvert(lookMatrix);

            rlMultMatrixf(MatrixToFloat(billboardMatrix));

            DrawCircleSector({0.0f,0.0f}, t->properties.radius, 0.0f, 360.0f, 10, trailcolor);

            rlPopMatrix();
        }
    }
}

inline void DrawCollider(const Collider& collider, const Transform& transform, Vector3 playerPos)
{
    std::vector<Vector3> vertices = GetTransformedVertices(collider.localVertices, transform);
    
    for(const std::pair<int, int>& edge : collider.edgesIndexes)
    {
        Vector3 startPos = vertices[edge.first] - playerPos;
        Vector3 endPos = vertices[edge.second] - playerPos;

        DrawLine3D(startPos, endPos, RED);
    }
}