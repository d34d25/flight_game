#include <iostream>

#include "raylib.h"

#include "scene.h"

#include <memory>

std::unique_ptr<Scene> testScene;

//leaving this for emscripten
void RunGame()
{

}

int main()
{
    float accumulator = 0.0f;

    float fixedDt = 1.0f / 60.0f;

    InitWindow(NATIVE_WIDTH, NATIVE_HEIGHT, "");

    LoadAssets();

    testScene = std::make_unique<Scene>();

    InitScene(*testScene);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        if(dt > 0.25f) dt = 0.25f;

        accumulator += dt;

        UpdatePlayerInput(testScene->player);

        while (accumulator >= fixedDt)
        {
            UpdateScene(*testScene, fixedDt);

            accumulator -= fixedDt;
        }

        BeginDrawing();

        ClearBackground(SKYBLUE);
        
        DrawScene(*testScene);

        DrawFPS(10,10);

        EndDrawing();
    }

    UnloadScene(*testScene);

    testScene.reset();

    CloseWindow();

    return 0;
}