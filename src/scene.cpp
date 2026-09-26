#include "scene.h"

#include <cstdio>

void InitScene(Scene &scene)
{
    scene.gameplayCanvas = LoadRenderTexture(CANVAS_WIDTH * SCALE, CANVAS_HEIGHT * SCALE);

    InitPlayer(scene.player);
    InitCamera();

    scene.lightDir = {0.5f, 1.0f, 0.75f};;

    scene.minIntensity = 0.4f;
    scene.maxIntensity = 1.0f;

    scene.flatShader = LoadShader("shaders/flat.vs", "shaders/flat.fs");

    scene.lightDirLoc = GetShaderLocation(scene.flatShader, "lightDir");
    scene.minIntensityLoc = GetShaderLocation(scene.flatShader, "minIntensity");
    scene.maxIntensityLoc = GetShaderLocation(scene.flatShader, "maxIntensity");

    SetShaderValue(scene.flatShader, scene.lightDirLoc, &scene.lightDir, SHADER_UNIFORM_VEC3);
    SetShaderValue(scene.flatShader, scene.maxIntensityLoc, &scene.maxIntensity, SHADER_UNIFORM_FLOAT);
    SetShaderValue(scene.flatShader, scene.minIntensityLoc, &scene.minIntensity, SHADER_UNIFORM_FLOAT);

    Model& playerModel = GetAircraftModel(scene.player.aircraft);

    for(int i = 0; i < playerModel.materialCount; i++)
    {
        if(i != 1) playerModel.materials[i].shader = scene.flatShader;
    }

    //sky sphere

    scene.skyRadius = 100.0f;

    Mesh skySphereMesh = GenMeshSphere(scene.skyRadius, 16, 16);

    scene.skySphereModel = LoadModelFromMesh(skySphereMesh);

    scene.baseColor = TRANSPARENT_BLUE;
    scene.topColor = DEEP_BLUE;
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

    scene.viewPos = camera.position;

    scene.fogDensity = 0.00025f;

    scene.fogShader = LoadShader("shaders/fog.vs", "shaders/fog.fs");

    scene.fogMinIntensityLoc = GetShaderLocation(scene.fogShader, "minIntensity");
    scene.fogMaxIntenistyLoc = GetShaderLocation(scene.fogShader, "maxIntensity");
    
    scene.fogLightDirLoc = GetShaderLocation(scene.fogShader, "lightDir");
    scene.fogColorLoc = GetShaderLocation(scene.fogShader, "fogColor");

    scene.fogDensityLoc = GetShaderLocation(scene.fogShader, "fogDensity");
    scene.viewPosLoc = GetShaderLocation(scene.fogShader, "viewPos");

    SetShaderValue(scene.fogShader, scene.fogLightDirLoc, &scene.lightDir, SHADER_UNIFORM_VEC3);
    SetShaderValue(scene.fogShader, scene.fogMinIntensityLoc, & scene.minIntensity, SHADER_UNIFORM_FLOAT);
    SetShaderValue(scene.fogShader, scene.fogMaxIntenistyLoc, & scene.maxIntensity, SHADER_UNIFORM_FLOAT);

    SetShaderValue(scene.fogShader, scene.fogColorLoc, &baseColor, SHADER_UNIFORM_VEC4);
    
    SetShaderValue(scene.fogShader, scene.fogDensityLoc, &scene.fogDensity, SHADER_UNIFORM_FLOAT);

    SetShaderValue(scene.fogShader, scene.viewPosLoc, &scene.viewPos, SHADER_UNIFORM_VEC3);

    InitTerrain(scene.terrain, scene.fogShader, DUNES);
}

void UpdateScene(Scene &scene, float dt)
{
    Player& player = scene.player;

    UpdatePlayerInput(player);

    UpdatePlayer(player, dt);

    UpdateBody(player.aircraft.body, dt);

    scene.viewPos = camera.position;

    SetShaderValue(scene.fogShader, scene.viewPosLoc, &scene.viewPos, SHADER_UNIFORM_VEC3);

    std::cout<<"fog den loc "<<scene.fogColorLoc<<"\n";

    if(!player.orbitCamera) UpdateChaseCamera(player, player.aircraft.body.transform, dt);
    else UpdateOrbitCamera(player, dt);

    const float playerY = player.aircraft.body.transform.translation.y;

    float compressionFactor = playerY * 0.001f;

    scene.maxHeight = scene.baseMaxHeight / (1.0f + compressionFactor);
    scene.minHeight = -scene.baseMinHeight / (1.0f + compressionFactor);

    SetShaderValue(scene.skyShader, scene.minHeightLoc, &scene.minHeight, SHADER_UNIFORM_FLOAT);
    SetShaderValue(scene.skyShader, scene.maxHeightLoc, &scene.maxHeight, SHADER_UNIFORM_FLOAT);

    std::cout<<"\n";

    printf("\nforward speed: %.3f   ", GetForwardSpeed(player));

    std::cout<<"thrust: "<<player.aircraft.thrust<<"\n";

    std::cout<<"missile fired: "<<player.aircraft.mslFired<<"\n";

    std::cout<<"mesh count: "<<GetAircraftModel(player.aircraft).meshCount<<"\n";
}

void DrawGameplay(Scene &scene)
{
    Player& player = scene.player;

    BeginTextureMode(scene.gameplayCanvas);

    ClearBackground(SKYBLUE);

    rlSetClipPlanes(10.0, 200.0);

    BeginMode3D(camera);

    DrawSky(scene);

    EndMode3D();

    glClear(GL_DEPTH_BUFFER_BIT);

    rlSetClipPlanes(60.0, 4000.0);

    BeginMode3D(camera);

    DrawTerrain(scene.terrain);

    EndMode3D();

    glClear(GL_DEPTH_BUFFER_BIT);
    
    rlSetClipPlanes(3.0, 1000.0);

    BeginMode3D(camera);

    DrawAircraft(player.aircraft);

    DrawMissiles(player.aircraft);

    DrawBullets(player.aircraft);

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

    Rectangle destGameplayRec = {
        offsetX, offsetY,
        NATIVE_WIDTH,
        NATIVE_HEIGHT
    };

    DrawTexturePro(
        scene.gameplayCanvas.texture,
        sourceGamplayRec,
        destGameplayRec,
        {0.0f,0.0f},
        0.0f,
        WHITE
    );
}

void UnloadScene(Scene &scene)
{
    UnloadShader(scene.flatShader);

    UnloadShader(scene.skyShader);

    UnloadShader(scene.fogShader);

    UnloadRenderTexture(scene.gameplayCanvas);

    DestroyTerrain(scene.terrain);
}
