#pragma once

#include <iostream>

#include "player.h"

struct Scene
{
    Player player;

    Shader flatShader;

    Vector3 lightDir;

    float minIntensity;
    float maxIntensity;

    int lightDirLoc;
    int minIntensityLoc;
    int maxIntensityLoc;
};

void InitScene(Scene& scene);

void UpdateScene(Scene& scene, float dt);

void DrawScene(Scene& scene);

void UnloadScene(Scene& scene);