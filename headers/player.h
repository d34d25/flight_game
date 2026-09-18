#pragma once

#include "aircraft.h"

constexpr float FAKE_GRAVITY = 8.0f;

constexpr float BANK = 0.2f;

constexpr float BANK_PITCH = 0.1f;

constexpr float STALL_FORCE = 1.0f;

constexpr float BASE_FOVY = 60.0f;

inline Camera3D camera = {};

inline Vector3 cameraOffset = {0.0f, 0.7f, -6.0f};

inline Vector3 lastFrameCameraOffset = {0.0f, 0.7f, -6.0f};

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

    bool stalling = true;
};

void UpdatePlayerInput(Player& player);

void UpdatePlayer(Player& player, float dt);

void UpdateCameraTransform(const Player& player ,const Transform &targerTransform, float dt);

inline void InitCamera()
{
    camera.fovy = BASE_FOVY;
    camera.projection = CAMERA_PERSPECTIVE;

    camera.target = {};
    camera.position = {};

    camera.up = {0.0f,1.0f,0.0f};
}

inline void InitPlayer(Player& player)
{
    player.aircraft = InitAircraft(F_15);
    player.aircraft.body.transform.translation.y = 200.0f;
}

inline float GetForwardSpeed(Player& player)
{
    const Body& body = player.aircraft.body;

    Vector3 forwardVec = GetWorldVectorFromLocalVector(body.transform.rotation, LOCAL_FORWARD);

    return Vector3DotProduct(body.linearVelocity, forwardVec);
}

inline void RemoveControlPlayer(Player& player)
{
    player.pitchUp = false;
    player.pitchDown = false;

    player.rollLeft = false;
    player.rollRight = false;

    player.yawLeft = false;
    player.yawRight = false;
}

inline void ResetInputPlayer(Player& player)
{
    RemoveControlPlayer(player);

    player.throttleUp = false;
    player.throttleDown = false;
}

inline float CalculateMobilityFactor(float forwardSpeed, const AircraftConfig& config)
{
    float mobilityLoseLow = config.idleSpeed * 0.75f;
    float mobilityLoseHigh = config.idleSpeed * 1.25f;

    float minMobilityFactor = 0.3f;
    float maxMobilityFactor = 0.4f;

    float mobilityFactor = 1.0f;

    if(forwardSpeed <= mobilityLoseLow)
    {
        mobilityFactor = 1 - (1 - minMobilityFactor) * (forwardSpeed - mobilityLoseLow) / (config.stallSpeed - mobilityLoseLow);

        mobilityFactor = Clamp(mobilityFactor, minMobilityFactor, 1.0f);
    }
    else if (forwardSpeed >= mobilityLoseHigh)
    {
        mobilityFactor = 1 - (1 - maxMobilityFactor) * (forwardSpeed - mobilityLoseHigh) / (config.maxSpeed - mobilityLoseHigh);

        mobilityFactor = Clamp(mobilityFactor, maxMobilityFactor, 1.0f);
    }

    return mobilityFactor;
}

inline float CalculateCameraFOVFactor(float forwardSpeed, const AircraftConfig& config)
{
    float fovThresholdLow = config.idleSpeed * 0.75f;
    float fovThresholdHigh = config.idleSpeed * 1.25f;

    float minFOVFactor = 0.85f;
    float maxFOVFactor = 1.15f;

    float fovFactor = 1.0f;

    if(forwardSpeed <= fovThresholdLow)
    {
        fovFactor = 1 - (1 - minFOVFactor) * (forwardSpeed - fovThresholdLow) / (config.stallSpeed - fovThresholdLow);

        fovFactor = Clamp(fovFactor, minFOVFactor, 1.0f);
    }
    else if (forwardSpeed >= fovThresholdHigh)
    {
        fovFactor = 1 - (1 - maxFOVFactor) * (forwardSpeed - fovThresholdHigh) / (config.maxSpeed - fovThresholdHigh);

        fovFactor = Clamp(fovFactor, 1.0f, maxFOVFactor);
    }

    return fovFactor;
}