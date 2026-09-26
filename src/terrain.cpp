#include "terrain.h"

void InitTerrainDB()
{
    TerrainProperties& dunes = terrainDB[DUNES];

    dunes.resolution = 256;
    dunes.scale = 600.0f;
    dunes.ySize = 150.0f;

    dunes.chunkSizeCorrectionFactor = 1.00157f;

    dunes.color = BEIGE;
    
    TerrainProperties& hills = terrainDB[HILLS];

    hills.resolution = 256;
    hills.scale = 200.0f;
    hills.ySize = 50.0f;

    hills.chunkSizeCorrectionFactor = 1.00165f;

    hills.color = DARKGREEN;
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

    if(
        terrain.chunkSize >= terrain.masterImg.width || 
        terrain.chunkSize >= terrain.masterImg.height ||
        terrain.chunkSize <= 0.0f
    )
    {
        UnloadImage(terrain.masterImg);
        
        std::cout<<"INVALID CHUNK SIZE, RETURNING\n";

        return;
    }

    int gap = 1;

    int esize = terrain.chunkSize - gap;

    for(int y = 0; y < terrain.masterImg.height - gap; y+= terrain.chunkSize)
    {
        for(int x = 0; x < terrain.masterImg.width - gap; x+= terrain.chunkSize)
        {
            int w = (x + terrain.chunkSize + gap <= terrain.masterImg.width) ? (terrain.chunkSize + gap) : (terrain.masterImg.width - x);
            int h = (y + terrain.chunkSize + gap <= terrain.masterImg.height) ? (terrain.chunkSize + gap) : (terrain.masterImg.height - y);

            Rectangle chunkRec = {
                (float)x,
                (float)y,
                (float)(w),
                (float)(h)
            };

            terrain.chunkImages.push_back(ImageFromImage(terrain.masterImg, chunkRec));
        }
    }

    int gridW = terrain.masterImg.width / terrain.chunkSize;

    for(int i = 0; i < terrain.chunkImages.size(); i++)
    {
        Image& chunkImg = terrain.chunkImages[i];

        Chunk chunk = {};

        float gridX = i % gridW;
        float gridY = i / gridW;

        float pixelX = (float)(gridX * terrain.chunkSize);
        float pixelY = (float)(gridY * terrain.chunkSize);

        float worldX = (pixelX / terrain.masterImg.width) * terrain.worldSize - (terrain.worldSize * 0.5f);
        float worldY = (pixelY / terrain.masterImg.height) * terrain.worldSize - (terrain.worldSize * 0.5f);
        
        float chunkWorldWidth = terrain.worldSize * ((float)terrain.chunkSize / terrain.masterImg.width) * config.chunkSizeCorrectionFactor;
        float chunkWorldHeight = terrain.worldSize * ((float)terrain.chunkSize / terrain.masterImg.height) * config.chunkSizeCorrectionFactor;

        chunk.offset = {worldX, worldY};

        Mesh chunkMesh = GenMeshHeightmap(
            chunkImg,
            {
                chunkWorldWidth,
                config.ySize,
                chunkWorldHeight
            }
        );

        chunk.model = LoadModelFromMesh(chunkMesh);

        for(int m = 0; m < chunk.model.materialCount; m++)
        {
            chunk.model.materials[m].shader = shader;
        }

        terrain.chunks.push_back(chunk);
    }
}

void DestroyTerrain(Terrain &terrain)
{
    for(Image& img : terrain.chunkImages)
    {
        UnloadImage(img);
    }

    terrain.chunkImages.clear();
    terrain.chunkImages.shrink_to_fit();

    for(Chunk& chunk : terrain.chunks)
    {
        UnloadModel(chunk.model);
    }

    terrain.chunks.clear();
    terrain.chunks.shrink_to_fit();

    UnloadImage(terrain.masterImg);
}
