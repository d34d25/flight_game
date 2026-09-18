#include <iostream>

#include "raylib.h"

#include "player.h"

int windowWidth = 1200;
int windowHeight = 800;

Player player;

//leaving this for emscripten
void RunGame()
{

}

int main()
{
    InitWindow(windowWidth, windowHeight, "");

    LoadAssets();

    InitPlayer(player);

    InitCamera();

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        UpdatePlayerInput(player);

        UpdatePlayer(player, dt);

        UpdateBody(player.aircraft.body, dt);

        UpdateCameraTransform(player, player.aircraft.body.transform, dt);

        std::cout<<"forward speed: "<<GetForwardSpeed(player)<<"\n";

        BeginDrawing();

        ClearBackground(SKYBLUE);

        BeginMode3D(camera);
        
        DrawGrid(1000,10);

        Matrix mScale = MatrixScale(1,1,1);

        Matrix mRotation = QuaternionToMatrix(player.aircraft.body.transform.rotation);

        Vector3& translation = player.aircraft.body.transform.translation;

        Matrix mTranslation = MatrixTranslate(translation.x, translation.y, translation.z);

        GetAircraftModel(player.aircraft).transform = MatrixMultiply(MatrixMultiply(mScale, mRotation), mTranslation);

        DrawModel(GetAircraftModel(player.aircraft), {0,0,0}, 1, WHITE);

        EndMode3D();

        DrawFPS(10,10);

        EndDrawing();
    }
    
    CloseWindow();

    return 0;
}