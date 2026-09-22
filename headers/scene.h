#pragma once

#include <iostream>

#include "player.h"

#include "rlgl.h"

constexpr float NATIVE_WIDTH = 1600;
constexpr float NATIVE_HEIGHT = 900;

constexpr float CANVAS_WIDTH = 256;
constexpr float CANVAS_HEIGHT = 144;

constexpr int SCALE = 4;

struct Scene
{
    Player player;

    Model skySphereModel;

    RenderTexture gameplayCanvas;

    Shader skyShader;

    Shader flatShader;

    Vector3 lightDir;

    //sky shader
    Color baseColor;
    Color topColor;
    Color bottomColor;

    float minHeight;
    float maxHeight;

    float baseMinHeight;
    float baseMaxHeight;

    float skyRadius;
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
};

void InitScene(Scene& scene);

void UpdateScene(Scene& scene, float dt);

void DrawGameplay(Scene& scene);

void DrawScene(Scene& scene);

void UnloadScene(Scene& scene);

inline void DrawAircraft(Aircraft& aircraft)
{
    Matrix mScale = MatrixScale(1,1,1);

    Matrix mRotation = QuaternionToMatrix(aircraft.body.transform.rotation);

    Vector3& translation = aircraft.body.transform.translation;

    Matrix mTranslation = MatrixTranslate(translation.x, translation.y, translation.z);

    Matrix finalMatrix = MatrixMultiply(MatrixMultiply(mScale, mRotation), mTranslation);

    Model& model = GetAircraftModel(aircraft);

    model.transform = finalMatrix;

    DrawModel(model, {0.0f,0.0f,0.0f}, 1, WHITE);
}

inline void DrawBullets(Aircraft& aircraft)
{
    for(Bullet* b : aircraft.bulletpool.activeBullets)
    {
        if(!b) continue;

        rlPushMatrix();

        rlTranslatef(b->transform.translation.x, b->transform.translation.y, b->transform.translation.z);

        rlMultMatrixf(MatrixToFloat(QuaternionToMatrix(b->transform.rotation)));

        DrawCubeV({0.0f,0.0f,0.0f}, {b->bulletSize * 0.5f, b->bulletSize * 0.5f,b->bulletSize}, YELLOW);

        rlPopMatrix();
    }
}

inline void DrawMissiles(Aircraft& aircraft)
{
    for(Missile* m : aircraft.missilePool.activeMissiles)
    {
        if(!m) continue;

        rlPushMatrix();

        rlTranslatef(m->body.transform.translation.x, m->body.transform.translation.y, m->body.transform.translation.z);

        rlMultMatrixf(MatrixToFloat(QuaternionToMatrix(m->body.transform.rotation)));

        DrawCubeV({0.0f,0.0f,0.0f}, {m->missileSize * 0.5f, m->missileSize * 0.5f,m->missileSize}, RAYWHITE);

        rlPopMatrix();
    }
}