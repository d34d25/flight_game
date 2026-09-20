#pragma once

#include "body.h"

constexpr float THRUST_RECOVERY_BELOW_IDLE = 10.0f; //when below thrust

constexpr float THRUST_RECOVERY_ABOVE_IDLE = 5.0f; //when above thrust

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

    //mobility
    float pitch;
    float roll;
    float yaw;
};

struct Aircraft
{
    AircraftType type;
    Body body;

    float thrust = 0.0f;
};

inline float GetThrustForDesiredSpeed(float desiredSpeed, float forwardDrag)
{
    return desiredSpeed * forwardDrag;
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

    f15.maxThrust = GetThrustForDesiredSpeed(f15.maxSpeed, f15.forwardDrag);
    f15.idleThrust = GetThrustForDesiredSpeed(f15.idleSpeed, f15.forwardDrag);

    f15.acceleration = 8.0f;
    f15.breakPower = 20.0f;

    f15.pitch = 1.25f;
    f15.roll = 4.5;
    f15.yaw = 0.35f;
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