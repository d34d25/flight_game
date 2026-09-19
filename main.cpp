#include <iostream>

#include "raylib.h"

#include "scene.h"

int windowWidth = 1200;
int windowHeight = 800;

Scene testScene;

//leaving this for emscripten
void RunGame()
{

}

int main()
{
    InitWindow(windowWidth, windowHeight, "");

    LoadAssets();

    InitScene(testScene);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        UpdateScene(testScene, dt);

        BeginDrawing();

        ClearBackground(SKYBLUE);

        BeginMode3D(camera);
        
        DrawScene(testScene);

        EndMode3D();

        DrawFPS(10,10);

        EndDrawing();
    }
    
    CloseWindow();

    UnloadScene(testScene);

    return 0;
}