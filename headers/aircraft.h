#pragma once

#include "body.h"

//forces
constexpr float FAKE_GRAVITY = 40.0f;

//torques
constexpr float BANK = 0.05f;

constexpr float BANK_PITCH = 0.03f;

constexpr float STALL_TORQUE = 0.5f;

enum AircraftType
{
    F_15,
    AIRCRAFT_COUNT
};

struct AircraftConfig
{
    Model model;

    Vector3 angularDrag;

    float forwardDrag;
    float sideDrag;

    //actual speed
    float maxSpeed;
    float idleSpeed;

    float stallSpeed;
    float recoverySpeed;

    //thust
    float maxThrust; //no need to modify directly
    float idleThrust; //no need to modify directly

    float acceleration;
    float breakPower;

    float recoveryAboveIdle;
    float recoveryBelowIdle;

    //mobility
    float pitch;
    float roll;
    float yaw;

    float gravity;

    float bank;
    float bankPitch;
    float stallTorque;
};

struct Aircraft
{
    AircraftType type;
    Body body;

    float thrust = 0.0f;
};

inline float GetDesiredValue(float desiredValue, float drag)
{
    return desiredValue * drag;
}

inline AircraftConfig aircraftsDB[AIRCRAFT_COUNT];

inline void LoadAssets()
{
    //F-15
    AircraftConfig& f15 = aircraftsDB[F_15];

    f15.model = LoadModel("assets/models/f-15.obj");
        
    f15.angularDrag = {2,2,2};

    f15.forwardDrag = 0.2f;
    f15.sideDrag = 4.0f;

    f15.maxSpeed = 170.0f;
    f15.idleSpeed = 100.0f;

    f15.stallSpeed = 30.0f;
    f15.recoverySpeed = 40.0f;

    f15.maxThrust = GetDesiredValue(f15.maxSpeed, f15.forwardDrag);
    f15.idleThrust = GetDesiredValue(f15.idleSpeed, f15.forwardDrag);

    f15.acceleration = GetDesiredValue(40, f15.forwardDrag);
    f15.breakPower = GetDesiredValue(100, f15.forwardDrag);

    f15.recoveryAboveIdle = GetDesiredValue(30, f15.forwardDrag);
    f15.recoveryBelowIdle = GetDesiredValue(25, f15.forwardDrag);

    f15.pitch = GetDesiredValue(0.45f, f15.angularDrag.x);
    f15.roll = GetDesiredValue(2.0f, f15.angularDrag.z);
    f15.yaw = GetDesiredValue(0.125f, f15.angularDrag.y);

    //common values

    for(int i = 0; i < AIRCRAFT_COUNT; i++)
    {
        AircraftConfig& config = aircraftsDB[i];

        config.gravity = GetDesiredValue(FAKE_GRAVITY, config.forwardDrag);

        config.bank = GetDesiredValue(BANK, config.angularDrag.y);

        config.bankPitch = GetDesiredValue(BANK_PITCH, config.angularDrag.x);

        config.stallTorque = GetDesiredValue(STALL_TORQUE, (config.angularDrag.x + config.angularDrag.y) * 0.5f);
    }
}

inline Body InitAircraftBody(AircraftType type)
{
    Body body = {};

    body.transform.rotation = QuaternionIdentity();
    body.angularDrag = aircraftsDB[type].angularDrag;
    body.forwardDrag = aircraftsDB[type].forwardDrag;
    body.sideDrag = aircraftsDB[type].sideDrag;

    return body;
}

inline Aircraft InitAircraft(AircraftType type)
{
    Aircraft aircraft = {};
    
    aircraft.type = type;

    aircraft.body = InitAircraftBody(aircraft.type);

    return aircraft;
}

inline Model& GetAircraftModel(const Aircraft& aircraft)
{
    return aircraftsDB[aircraft.type].model;
}

inline const AircraftConfig& GetAircraftConfig(const Aircraft& aircraft)
{
    return aircraftsDB[aircraft.type];
}