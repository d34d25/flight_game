#pragma once

#include <iostream>

#include <vector>

#include "raylib.h"

#include <math.h>

#include "helpers.h"

enum TerrainType
{
    DUNES,
    HILLS,
    TERRAIN_COUNT
};

struct TerrainProperties
{
    Color color;

    float ySize;
    float scale;
    int resolution;
};

inline TerrainProperties terrainDB[TERRAIN_COUNT];

void InitTerrainDB();

struct Terrain
{
    Image masterImg;

    Model model;

    Color color;
};

void InitTerrain(Terrain& terrain, Shader& shader, TerrainType type);

void DestroyTerrain(Terrain& terrain);

inline void DrawTerrain(Terrain& terrain, Vector3 playerPos)
{
    DrawModel(terrain.model,{-playerPos.x,-playerPos.y,-playerPos.z},1,terrain.color);
}
