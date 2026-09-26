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
    InitWindow(NATIVE_WIDTH, NATIVE_HEIGHT, "");

    LoadAssets();

    testScene = std::make_unique<Scene>();

    InitScene(*testScene);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        UpdateScene(*testScene, dt);

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