#include "terrain.h"

void InitTerrainDB()
{
    TerrainProperties& dunes = terrainDB[DUNES];

    dunes.resolution = 256;
    dunes.scale = 600.0f;
    dunes.ySize = 150.0f;

    dunes.color = BEIGE;
    
    TerrainProperties& hills = terrainDB[HILLS];

    hills.resolution = 256;
    hills.scale = 200.0f;
    hills.ySize = 50.0f;

    hills.color = Color{40,120,80,255};
}

void InitTerrain(Terrain &terrain, Shader &shader, TerrainType type)
{
    InitTerrainDB();

    TerrainProperties& config = terrainDB[type];

    terrain.color = config.color;

    terrain.masterImg = GenImagePerlinNoise(
        config.resolution,
        config.resolution,
        0,
        0,
        config.scale
    );

    terrain.model = LoadModelFromMesh(GenMeshHeightmap(terrain.masterImg, {WORLD_SIZE, config.ySize, WORLD_SIZE}));

    terrain.model.materials[0].shader = shader;
}

void DestroyTerrain(Terrain &terrain)
{
    UnloadModel(terrain.model);

    UnloadImage(terrain.masterImg);
}
