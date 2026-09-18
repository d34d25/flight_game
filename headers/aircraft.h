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
    float mobilityLoseLow;
    float mobilityLoseHigh;

    float stallSpeed;
    float recoverySpeed;

    //thust
    float maxThrust;
    float idleThrust;

    float acceleration;
    float breakPower;

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

inline AircraftConfig aircraftsDB[AIRCRAFT_COUNT];

inline void LoadAssets()
{
    aircraftsDB[F_15] = {

        .model = LoadModel("assets/models/f-15.obj"),
        
        .angularDrag = {2,2,2},

        .forwardDrag = 0.2f,
        .sideDrag = 4.0f,

        .stallSpeed = 20.0f,
        .recoverySpeed = 30.0f,

        .maxThrust = 60.0f,
        .idleThrust = 20.0f,

        .acceleration = 8.0f,
        .breakPower = 20.0f,

        .pitch = 2.0f,
        .roll = 5.0f,
        .yaw = 1.0f
    };
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