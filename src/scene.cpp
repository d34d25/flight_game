#include "scene.h"

void InitScene(Scene &scene)
{
    InitPlayer(scene.player);
    InitCamera();

    scene.lightDir = {0.5f, 1.0f, 0.3f};;

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

    scene.baseColor = {150, 150, 255, 255};
    scene.topColor = {25,25,100,255};
    scene.bottomColor = WHITE;

    scene.maxHeight = scene.skyRadius;
    scene.minHeight = -scene.skyRadius;

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
}

void UpdateScene(Scene &scene, float dt)
{
    Player& player = scene.player;

    UpdatePlayerInput(player);

    UpdatePlayer(player, dt);

    UpdateBody(player.aircraft.body, dt);

    UpdateCameraTransform(player, player.aircraft.body.transform, dt);

    const float playerY = player.aircraft.body.transform.translation.y;

    float compressionFactor = playerY * 0.0001f;

    scene.maxHeight = scene.skyRadius / (1.0f + compressionFactor);
    scene.minHeight = -scene.skyRadius / (1.0f + compressionFactor);

    SetShaderValue(scene.skyShader, scene.minHeightLoc, &scene.minHeight, SHADER_UNIFORM_FLOAT);
    SetShaderValue(scene.skyShader, scene.maxHeightLoc, &scene.maxHeight, SHADER_UNIFORM_FLOAT);

    //std::cout<<"forward speed: "<<GetForwardSpeed(player)<<"\n";

    //std::cout<<"loc: "<<GetShaderLocation(scene.flatShader, "maxIntensity")<<"\n";
}

void DrawScene(Scene &scene)
{
    rlPushMatrix();

    rlDisableBackfaceCulling();
    rlDisableDepthMask();

    Matrix originalView = rlGetMatrixModelview();

    Matrix view = GetCameraMatrix(camera);

    view.m12 = 0.0f; view.m13 = 0.0f; view.m14 = 0.0f;

    rlSetMatrixModelview(view);

    DrawModel(scene.skySphereModel, {0,0,0}, 1, RAYWHITE);

    rlEnableDepthMask();
    rlEnableBackfaceCulling();

    rlSetMatrixModelview(originalView);

    rlPopMatrix();

    Player& player = scene.player;

    DrawGrid(1000,10);

    Matrix mScale = MatrixScale(1,1,1);

    Matrix mRotation = QuaternionToMatrix(player.aircraft.body.transform.rotation);

    Vector3& translation = player.aircraft.body.transform.translation;

    Matrix mTranslation = MatrixTranslate(translation.x, translation.y, translation.z);

    Matrix finalMatrix = MatrixMultiply(MatrixMultiply(mScale, mRotation), mTranslation);

    GetAircraftModel(player.aircraft).transform = finalMatrix;

    const Model& playerModel = GetAircraftModel(player.aircraft);

    DrawModel(playerModel, {0,0,0}, 1, WHITE);
}

void UnloadScene(Scene &scene)
{
    UnloadShader(scene.flatShader);
}
