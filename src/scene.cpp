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
}

void UpdateScene(Scene &scene, float dt)
{
    Player& player = scene.player;

    UpdatePlayerInput(player);

    UpdatePlayer(player, dt);

    UpdateBody(player.aircraft.body, dt);

    UpdateCameraTransform(player, player.aircraft.body.transform, dt);

    //std::cout<<"forward speed: "<<GetForwardSpeed(player)<<"\n";

    //std::cout<<"loc: "<<GetShaderLocation(scene.flatShader, "maxIntensity")<<"\n";
}

void DrawScene(Scene &scene)
{
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
