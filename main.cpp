#include <iostream>

#include "raylib.h"

#include "scene.h"

Scene testScene;

//leaving this for emscripten
void RunGame()
{

}

int main()
{
    InitWindow(NATIVE_WIDTH, NATIVE_HEIGHT, "");

    LoadAssets();

    InitScene(testScene);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        UpdateScene(testScene, dt);

        BeginDrawing();

        ClearBackground(SKYBLUE);
        
        DrawScene(testScene);

        DrawFPS(10,10);

        EndDrawing();
    }
    
    CloseWindow();

    UnloadScene(testScene);

    return 0;
}