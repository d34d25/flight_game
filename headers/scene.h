#pragma once

#include <iostream>

#include "player.h"

#include "rlgl.h"

struct Scene
{
    Player player;

    Model skySphereModel;

    Shader skyShader;

    Shader flatShader;

    Vector3 lightDir;

    //sky shader
    Color baseColor;
    Color topColor;
    Color bottomColor;

    float minHeight;
    float maxHeight;

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

void DrawScene(Scene& scene);

void UnloadScene(Scene& scene);