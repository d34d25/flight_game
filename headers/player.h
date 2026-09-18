#pragma once

#include "aircraft.h"

constexpr float FAKE_GRAVITY = 20.0f;

constexpr float BANK = 0.2f;

constexpr float BANK_PITCH = 0.1f;

constexpr float STALL_FORCE = 1.0f;

inline Camera3D camera = {};

inline Vector3 cameraOffset = {0.0f, 0.7f, -6.0f};

struct Player
{
    Aircraft aircraft;

    bool pitchUp = false;
    bool pitchDown = false;
    
    bool rollRight = false;
    bool rollLeft = false;

    bool yawRight = false;
    bool yawLeft = false;

    bool throttleUp = false;
    bool throttleDown = false;
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

inline float GetForwardSpeed(Player& player)
{
    const Body& body = player.aircraft.body;

    Vector3 forwardVec = GetWorldVectorFromLocalVector(body.transform.rotation, LOCAL_FORWARD);

    return Vector3DotProduct(body.linearVelocity, forwardVec);
}