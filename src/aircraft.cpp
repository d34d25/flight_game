#include "aircraft.h"

void InitAircraftDB()
{
    //F-15
    AircraftConfig& f15 = aircraftsDB[F_15];

    f15.model = LoadModel("assets/models/f-15s.obj");

    f15.engineMaterial = 13;
        
    f15.angularDrag = {2,2,2};

    f15.forwardDrag = 0.2f;
    f15.sideDrag = 4.0f;;

    f15.maxSpeed = 250.0f;
    f15.idleSpeed = 140.0f;

    f15.stallSpeed = 70.0f;
    f15.recoverySpeed = 80.0f;

    f15.maxThrust = GetDesiredValue(f15.maxSpeed, f15.forwardDrag);
    f15.idleThrust = GetDesiredValue(f15.idleSpeed, f15.forwardDrag);

    f15.acceleration = GetDesiredValue(40, f15.forwardDrag);
    f15.breakPower = GetDesiredValue(100, f15.forwardDrag);

    f15.recoveryAboveIdle = GetDesiredValue(30, f15.forwardDrag);
    f15.recoveryBelowIdle = GetDesiredValue(25, f15.forwardDrag);

    f15.pitch = GetDesiredValue(0.6f, f15.angularDrag.x);
    f15.roll = GetDesiredValue(2.2f, f15.angularDrag.z);
    f15.yaw = GetDesiredValue(0.15f, f15.angularDrag.y);

    f15.gunType = VULKAN;

    f15.gunOffset = {-0.25f, 0.1f, -6.1f};

    f15.mslType = STANDARD_MSL;
    
    f15.mslOffset = {-1.0f, -0.5f, -3.0f};

    f15.collider = CreatePrismatoid(
        1.25f, //base w
        0.25f, //base l
        0.5f, //top w
        0.2f, // top h
        2.5f, // height
        QuaternionFromAxisAngle(LOCAL_RIGHT, 1.57f),
        {0.0f, 0.05f, 0.0f}
    );

    AircraftConfig& sutype = aircraftsDB[SUTYPE];

    sutype.model = LoadModel("assets/models/sutype.obj");

    sutype.engineMaterial = 4;
        
    sutype.angularDrag = {2,2,2};

    sutype.forwardDrag = 0.2f;
    sutype.sideDrag = 4.0f;;

    sutype.maxSpeed = 250.0f;
    sutype.idleSpeed = 140.0f;

    sutype.stallSpeed = 70.0f;
    sutype.recoverySpeed = 80.0f;

    sutype.maxThrust = GetDesiredValue(sutype.maxSpeed, sutype.forwardDrag);
    sutype.idleThrust = GetDesiredValue(sutype.idleSpeed, sutype.forwardDrag);

    sutype.acceleration = GetDesiredValue(40, sutype.forwardDrag);
    sutype.breakPower = GetDesiredValue(100, sutype.forwardDrag);

    sutype.recoveryAboveIdle = GetDesiredValue(30, sutype.forwardDrag);
    sutype.recoveryBelowIdle = GetDesiredValue(25, sutype.forwardDrag);

    sutype.pitch = GetDesiredValue(0.6f, sutype.angularDrag.x);
    sutype.roll = GetDesiredValue(2.2f, sutype.angularDrag.z);
    sutype.yaw = GetDesiredValue(0.15f, sutype.angularDrag.y);

    sutype.gunType = VULKAN;

    sutype.gunOffset = {-0.25f, 0.1f, -6.1f};

    sutype.mslType = STANDARD_MSL;
    
    sutype.mslOffset = {-1.0f, -0.5f, -3.0f};

    sutype.collider = CreatePrismatoid(
        1.25f, //base w
        0.25f, //base l
        0.5f, //top w
        0.2f, // top h
        2.5f, // height
        QuaternionFromAxisAngle(LOCAL_RIGHT, 1.57f),
        {0.0f, 0.05f, 0.0f}
    );

    //common values

    for(int i = 0; i < AIRCRAFT_COUNT; i++)
    {
        AircraftConfig& config = aircraftsDB[i];

        config.gravity = GetDesiredValue(FAKE_GRAVITY, config.forwardDrag);

        config.bank = GetDesiredValue(BANK, config.angularDrag.y);

        config.bankPitch = GetDesiredValue(BANK_PITCH, config.angularDrag.x);
    }
}