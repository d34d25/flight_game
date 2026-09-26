#pragma once

#include <iostream>

#include <vector>

#include "raylib.h"

#include <math.h>

const int SIZE_X_Z = 5000;

const int SIZE_Y = 50;

enum TerrainType
{
    DUNES,
    HILLS,
    TERRAIN_COUNT
};

struct TerrainProperties
{
    Color color;

    float chunkSizeCorrectionFactor;

    float ySize;
    float scale;
    int resolution;
};

inline TerrainProperties terrainDB[TERRAIN_COUNT];

void InitTerrainDB();

struct Chunk
{
    Model model;
    Vector2 offset;
};

struct Terrain
{
    Image masterImg;

    std::vector<Image> chunkImages;

    std::vector<Chunk> chunks;

    Color color;

    float worldSize = 50000;

    int chunkSize = 8;
};

void InitTerrain(Terrain& terrain, Shader& shader, TerrainType type);

void DestroyTerrain(Terrain& terrain);

inline void DrawTerrain(Terrain& terrain)
{
    for(Chunk& chunk : terrain.chunks)
    {
        DrawModel(chunk.model, 
            {
                floorf(chunk.offset.x), 
                0.0f, 
                floorf(chunk.offset.y)
            },
            1, 
            terrain.color
        );
    }
}

/*

dessert dunes

res 64

scale 600

size 10000 x z

size 100 y

desert / plains

res 256

scale 200

size 50000 x z

size 50 y

*/
