#pragma once

#include "body.h"

#include "weapons.h"

#include "collisions.h"

//forces
constexpr float FAKE_GRAVITY = 40.0f;

//torques
constexpr float BANK = 0.05f;

constexpr float BANK_PITCH = 0.03f;

constexpr float STALL_TORQUE = 0.5f;

enum AircraftType
{
    F_15,
    SUTYPE,
    AIRCRAFT_COUNT
};

struct AircraftConfig
{
    Model model;

    Collider collider;

    Vector3 gunOffset;

    Vector3 mslOffset;

    Vector3 angularDrag;

    BulletType gunType;

    MissileType mslType;

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

    int engineMaterial;
};

struct Aircraft
{
    Body body;

    BulletPool bulletpool;

    MissilePool missilePool;

    Color debugColor = WHITE;
    
    AircraftType type;

    float thrust = 0.0f;

    int mslFired = 0;

    int hardpoints = 2;
};

inline AircraftConfig aircraftsDB[AIRCRAFT_COUNT];

void InitAircraftDB();

inline void LoadAssets()
{
    InitGunDB();

    InitMissileDB();

    InitTrailDB();

    InitAircraftDB();
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

    InitBulletPool(aircraft.bulletpool, gunsDB[aircraftsDB[type].gunType], 50);

    InitMissilePool(aircraft.missilePool, missilesDB[aircraftsDB[type].mslType], 30);

    return aircraft;
}

inline Model GetAircraftModelValue(const Aircraft& aircraft)
{
    return aircraftsDB[aircraft.type].model;
}

inline Model& GetAircraftModel(const Aircraft& aircraft)
{
    return aircraftsDB[aircraft.type].model;
}

inline const AircraftConfig& GetAircraftConfig(const Aircraft& aircraft)
{
    return aircraftsDB[aircraft.type];
}