#pragma once

#include "aircraft.h"

inline Camera3D camera = {};

inline Vector3 cameraOffset = {0.0f, 0.7f, -6.0f};

struct Player
{
    Aircraft aircraft;

    bool pitchUp;
    bool pitchDown;
    
    bool rollRight;
    bool rollLeft;

    bool yawRight;
    bool yawLeft;

    bool throttleUp;
    bool throttleDown;
};

void UpdatePlayerInput(Player& player);

void UpdatePlayer(Player& player, float dt);

void UpdateCameraTransform(const Transform &targerTransform, float dt);

inline void InitCamera()
{
    camera.fovy = 60;
    camera.projection = CAMERA_PERSPECTIVE;

    camera.target = {};
    camera.position = {};

    camera.up = {0.0f,1.0f,0.0f};
}

inline void InitPlayer(Player& player)
{
    player.aircraft = InitAircraft(F_15);
}