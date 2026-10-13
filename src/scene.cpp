#include "scene.h"

#include <cstdio>

void InitScene(Scene &scene)
{
    scene.terrainCanvas = LoadRenderTexture(CANVAS_WIDTH * SCALE, CANVAS_HEIGHT * SCALE);
    scene.gameplayCanvas = LoadRenderTexture(CANVAS_WIDTH * SCALE, CANVAS_HEIGHT * SCALE);
    scene.backgroundCanvas = LoadRenderTexture(CANVAS_WIDTH * SCALE, CANVAS_HEIGHT * SCALE);

    InitPlayer(scene.player);
    InitCamera();

    scene.lightDir = {0.5f, 1.0f, 0.75f};;

    scene.minIntensity = 0.6f;
    scene.maxIntensity = 1.0f;

    scene.flatShader = LoadShader("shaders/old/flat.vs", "shaders/old/flat.fs");

    scene.lightDirLoc = GetShaderLocation(scene.flatShader, "lightDir");
    scene.minIntensityLoc = GetShaderLocation(scene.flatShader, "minIntensity");
    scene.maxIntensityLoc = GetShaderLocation(scene.flatShader, "maxIntensity");

    SetShaderValue(scene.flatShader, scene.lightDirLoc, &scene.lightDir, SHADER_UNIFORM_VEC3);
    SetShaderValue(scene.flatShader, scene.maxIntensityLoc, &scene.maxIntensity, SHADER_UNIFORM_FLOAT);
    SetShaderValue(scene.flatShader, scene.minIntensityLoc, &scene.minIntensity, SHADER_UNIFORM_FLOAT);

    Model& playerModel = GetAircraftModel(scene.player.aircraft);

    const AircraftConfig& config = aircraftsDB[scene.player.aircraft.type];

    for(int i = 0; i < playerModel.materialCount; i++)
    {
        //if(i != config.engineMaterial) playerModel.materials[i].shader = scene.flatShader;
    }

    renderCamera = camera;

    //sky sphere

    scene.skyRadius = 100.0f;

    Mesh skySphereMesh = GenMeshSphere(scene.skyRadius, 16, 16);

    scene.skySphereModel = LoadModelFromMesh(skySphereMesh);

    scene.baseColor = SKYBLUE;
    scene.topColor = BLUE;
    scene.bottomColor = WHITE;
    
    scene.maxHeight = scene.skyRadius * 0.75f;
    scene.minHeight = 0.0f;

    scene.baseMaxHeight = scene.maxHeight;
    scene.baseMinHeight = scene.minHeight;

    scene.skyShader = LoadShader("shaders/sky.vs", "shaders/sky.fs");

    scene.baseColorLoc = GetShaderLocation(scene.skyShader, "baseColor");
    scene.topColorLoc = GetShaderLocation(scene.skyShader, "topColor");
    scene.bottomColorLoc = GetShaderLocation(scene.skyShader, "bottomColor");
    scene.minHeightLoc = GetShaderLocation(scene.skyShader, "minHeight");
    scene.maxHeightLoc = GetShaderLocation(scene.skyShader, "maxHeight");

    Vector4 baseColor = ColorNormalize(scene.baseColor);
    Vector4 topColor = ColorNormalize(scene.topColor);
    Vector4 bottomColor = ColorNormalize(scene.bottomColor);

    SetShaderValue(scene.skyShader, scene.baseColorLoc, &baseColor, SHADER_UNIFORM_VEC4);
    SetShaderValue(scene.skyShader, scene.topColorLoc, &topColor, SHADER_UNIFORM_VEC4);
    SetShaderValue(scene.skyShader, scene.bottomColorLoc, &bottomColor, SHADER_UNIFORM_VEC4);

    SetShaderValue(scene.skyShader, scene.minHeightLoc, &scene.minHeight, SHADER_UNIFORM_FLOAT);
    SetShaderValue(scene.skyShader, scene.maxHeightLoc, &scene.maxHeight, SHADER_UNIFORM_FLOAT);

    for(int i = 0; i < scene.skySphereModel.materialCount; i++)
    {
        scene.skySphereModel.materials[i].shader = scene.skyShader;
    }

    //fog Sahder

    scene.fogStart = 2500.0f;
    scene.fogEnd = 4000.0f;

    scene.viewPos = renderCamera.position;

    scene.fogDensity = 0.00025f;

    scene.fogScrollShader = LoadShader("shaders/fog_scroll.vs", "shaders/fog_scroll.fs");

    scene.fogColorLoc = GetShaderLocation(scene.fogScrollShader, "fogColor");

    scene.fogDensityLoc = GetShaderLocation(scene.fogScrollShader, "fogDensity");
    scene.viewPosLoc = GetShaderLocation(scene.fogScrollShader, "viewPos");

    scene.fogStartLoc = GetShaderLocation(scene.fogScrollShader, "fogStart");
    scene.fogEndLoc = GetShaderLocation(scene.fogScrollShader, "fogEnd");

    scene.playerXZLoc = GetShaderLocation(scene.fogScrollShader, "playerXZ");

    scene.scrollingLoc = GetShaderLocation(scene.fogScrollShader, "scrollingFactor");

    SetShaderValue(scene.fogScrollShader, scene.fogColorLoc, &baseColor, SHADER_UNIFORM_VEC4);
    
    SetShaderValue(scene.fogScrollShader, scene.fogDensityLoc, &scene.fogDensity, SHADER_UNIFORM_FLOAT);

    SetShaderValue(scene.fogScrollShader, scene.viewPosLoc, &scene.viewPos, SHADER_UNIFORM_VEC3);

    SetShaderValue(scene.fogScrollShader, scene.fogStartLoc, &scene.fogStart, SHADER_UNIFORM_FLOAT);
    SetShaderValue(scene.fogScrollShader, scene.fogEndLoc, &scene.fogEnd, SHADER_UNIFORM_FLOAT);

    float scrollingFactor = 1.0f / WORLD_SIZE;

    SetShaderValue(scene.fogScrollShader, scene.scrollingLoc, &scrollingFactor, SHADER_UNIFORM_FLOAT);

    Vector2 playerXZ = {
        scene.player.aircraft.body.transform.translation.x,
        scene.player.aircraft.body.transform.translation.z
    };

    SetShaderValue(scene.fogScrollShader, scene.playerXZLoc, &playerXZ, SHADER_ATTRIB_VEC2);

    scene.floor = LoadModelFromMesh(GenMeshPlane(WORLD_SIZE, WORLD_SIZE, 128,128));

    Image floorImg = GenImageChecked(250,250, 1, 1, WHITE, VERY_LIGHT_GRAY);

    Texture2D floorTexture = LoadTextureFromImage(floorImg);

    SetTextureWrap(floorTexture, TEXTURE_WRAP_REPEAT);

    UnloadImage(floorImg);

    scene.floor.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = floorTexture;

    scene.floor.materials[0].shader = scene.fogScrollShader;

    //debug

    scene.obstacleCollider = CreatePrismatoid(50,50,20,20,1000);
    scene.obstacleTransform = {};
    scene.obstacleTransform.rotation = QuaternionIdentity();
    scene.obstacleTransform.translation = scene.player.aircraft.body.transform.translation + Vector3{0,0, 1600};
    scene.obstacleTransform.scale = {1,1,1};
}

void UpdateScene(Scene &scene, float dt)
{
    Player& player = scene.player;

    UpdatePlayer(player, dt);

    UpdateBody(player.aircraft.body, dt);

    Vector3 playerPos = player.aircraft.body.transform.translation;

    scene.playerCollidedTerrain = playerPos.y <= 0.0f;
    
    scene.playerCollidedSATCCD = SAT3DCCD(

        aircraftsDB[player.aircraft.type].collider,
        player.aircraft.body.transform,
        player.aircraft.body.linearVelocity + player.aircraft.body.angularVelocity,

        scene.obstacleCollider, 
        scene.obstacleTransform,
        {0,0,0},
        
        dt
    );

    if(scene.playerCollidedTerrain)
    {
        player.aircraft.debugColor = RED;
    }

    if(scene.playerCollidedSATCCD)
    {
        player.aircraft.debugColor = GREEN;
    }
    
    if(!scene.playerCollidedSATCCD && !scene.playerCollidedTerrain)
    {
        player.aircraft.debugColor = WHITE;
    }

    scene.viewPos = renderCamera.position;

    SetShaderValue(scene.fogScrollShader, scene.viewPosLoc, &scene.viewPos, SHADER_UNIFORM_VEC3);

    if(!player.orbitCamera) UpdateChaseCamera(player, player.aircraft.body.transform, dt);
    else UpdateOrbitCamera(player, dt);

    const float playerY = player.aircraft.body.transform.translation.y;

    float compressionFactor = playerY * 0.001f;

    scene.maxHeight = scene.baseMaxHeight / (1.0f + compressionFactor);
    scene.minHeight = -scene.baseMinHeight / (1.0f + compressionFactor);

    SetShaderValue(scene.skyShader, scene.minHeightLoc, &scene.minHeight, SHADER_UNIFORM_FLOAT);
    SetShaderValue(scene.skyShader, scene.maxHeightLoc, &scene.maxHeight, SHADER_UNIFORM_FLOAT);

    Vector2 playerXZ = {
        playerPos.x,
        playerPos.z
    };

    SetShaderValue(scene.fogScrollShader, scene.playerXZLoc, &playerXZ, SHADER_ATTRIB_VEC2);

    renderCamera = camera;

    renderCamera.position -= playerPos;

    renderCamera.target -= playerPos;

    /*std::cout<<"\n";

    printf("\nforward speed: %.3f   ", GetForwardSpeed(player));

    std::cout<<"thrust: "<<player.aircraft.thrust<<"\n";

    std::cout<<"missile fired: "<<player.aircraft.mslFired<<"\n";

    std::cout<<"mesh count: "<<GetAircraftModel(player.aircraft).meshCount<<"\n";*/
}

void DrawGameplay(Scene &scene)
{
    Player& player = scene.player;

    Vector3 playerPos = player.aircraft.body.transform.translation;

    ClearBackground(SKYBLUE);

    BeginTextureMode(scene.backgroundCanvas);

    BeginMode3D(renderCamera);

    ClearBackground(BLANK);

    DrawSky(scene);

    DrawBG();

    EndMode3D();

    EndTextureMode();

    //terrain
    BeginTextureMode(scene.terrainCanvas);

    ClearBackground(BLANK);

    rlSetClipPlanes(3.0, 4000.0);

    BeginMode3D(renderCamera);

    DrawModel(scene.floor,{0,-playerPos.y,0},1, GREEN);

    DrawCollider(scene.obstacleCollider, scene.obstacleTransform, playerPos);

    EndMode3D();

    EndTextureMode();

    //gameplay

    BeginTextureMode(scene.gameplayCanvas);

    ClearBackground(BLANK);
    
    rlSetClipPlanes(3.0, 500.0);

    BeginMode3D(renderCamera);

    DrawPlayer(player.aircraft);

    //DrawCollider(aircraftsDB[player.aircraft.type].collider, player.aircraft.body.transform, playerPos);

    if(scene.playerCollidedTerrain)
    {
        DrawSphere({0,0,0}, 0.1f, RED);
    }

    DrawMissiles(player.aircraft, playerPos);

    DrawBullets(player.aircraft, playerPos);

    EndMode3D();

    EndTextureMode();
}

void DrawScene(Scene &scene)
{
    DrawGameplay(scene);

    ClearBackground(BLACK);

    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();

    float scale = fminf(screenW / NATIVE_WIDTH, screenH / NATIVE_HEIGHT);

    float offsetX = (screenW - NATIVE_WIDTH * scale) * 0.5f;
    float offsetY = (screenH - NATIVE_HEIGHT * scale) * 0.5f;
    
    Rectangle sourceGamplayRec = {
        0, 0, 
        (float)scene.gameplayCanvas.texture.width,
        (float)-scene.gameplayCanvas.texture.height
    };

    Rectangle sourceTerrainRec = {
        0, 0,
        (float)scene.terrainCanvas.texture.width,
        (float)-scene.terrainCanvas.texture.height
    };

    Rectangle sourceBackgroundRec = {
        0, 0,
        (float)scene.backgroundCanvas.texture.width,
        (float)-scene.backgroundCanvas.texture.height
    };

    Rectangle destRec = {
        offsetX, offsetY,
        NATIVE_WIDTH,
        NATIVE_HEIGHT
    };

    DrawTexturePro(
        scene.backgroundCanvas.texture,
        sourceBackgroundRec,
        destRec,
        {0.0f,0.0f},
        0.0f,
        WHITE
    );
    
    DrawTexturePro(
        scene.terrainCanvas.texture,
        sourceTerrainRec,
        destRec,
        {0.0f,0.0f},
        0.0f,
        WHITE
    );

    DrawTexturePro(
        scene.gameplayCanvas.texture,
        sourceGamplayRec,
        destRec,
        {0.0f,0.0f},
        0.0f,
        WHITE
    );
}

void UnloadScene(Scene &scene)
{
    UnloadShader(scene.flatShader);

    UnloadShader(scene.skyShader);

    UnloadShader(scene.fogScrollShader);

    UnloadRenderTexture(scene.gameplayCanvas);

    //DestroyTerrain(scene.terrain);

    UnloadModel(scene.floor);
}
