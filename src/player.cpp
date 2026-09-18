#include "player.h"

void UpdatePlayerInput(Player &player)
{
    player.pitchUp = IsKeyDown(KEY_DOWN);
    player.pitchDown = IsKeyDown(KEY_UP);

    player.rollLeft = IsKeyDown(KEY_LEFT);
    player.rollRight = IsKeyDown(KEY_RIGHT);

    player.yawLeft = IsKeyDown(KEY_A);
    player.yawRight = IsKeyDown(KEY_D);

    player.throttleUp = IsKeyDown(KEY_W);
    player.throttleDown = IsKeyDown(KEY_S);
}

void UpdatePlayer(Player &player, float dt)
{
    Body& body = player.aircraft.body;

    const AircraftConfig& config = GetAircraftConfig(player.aircraft);

    float& thrust = player.aircraft.thrust;

    if(player.pitchDown) ApplyTorqueLocal(body, LOCAL_RIGHT, config.pitch);
    else if(player.pitchUp) ApplyTorqueLocal(body, LOCAL_RIGHT, -config.pitch);

    if(player.rollLeft) ApplyTorqueLocal(body, LOCAL_FORWARD, -config.roll);
    else if(player.rollRight) ApplyTorqueLocal(body, LOCAL_FORWARD, config.roll);

    if(player.yawRight) ApplyTorqueLocal(body, LOCAL_UP, -config.yaw);
    else if(player.yawLeft) ApplyTorqueLocal(body, LOCAL_UP, config.yaw);

    if(player.throttleUp)
    {
        thrust += config.acceleration * dt;

        if(thrust >= config.maxThrust) thrust = config.maxThrust;
    }
    else if(player.throttleDown)
    {
        thrust -= config.breakPower * dt;

        if(thrust <= 0.0f) thrust = 0.0f;
    }
    else
    {
        if(thrust < config.idleThrust)
        {
            thrust += THRUST_RECOVERY_BELOW_IDLE * dt;

            if(thrust >= config.idleThrust) thrust = config.idleThrust;
        }
        else
        {
            thrust -= THRUST_RECOVERY_ABOVE_IDLE * dt;

            if(thrust <= config.idleThrust) thrust = config.idleThrust;
        }
    }

    ApplyForceLocal(body, LOCAL_FORWARD, thrust);

    //plane physics

    const Quaternion& rotation = body.transform.rotation;

    //drag / frake gravity

    Vector3 bodyForward = GetWorldVectorFromLocalVector(rotation, LOCAL_FORWARD);

    float dotForwardUp = Vector3DotProduct(bodyForward, LOCAL_UP);

    float forwardSpeed = Vector3DotProduct(body.linearVelocity, bodyForward);

    float upDragTolerance = 0.1f;

    if(dotForwardUp > upDragTolerance)
    {
        if(forwardSpeed > 0.0f) ApplyForceLocal(body, LOCAL_BACKWARD, FAKE_GRAVITY * dotForwardUp);
    }
    else if(dotForwardUp < -upDragTolerance)
    {
        ApplyForceLocal(body, LOCAL_BACKWARD, FAKE_GRAVITY * dotForwardUp);
    }

    //fake banking

    Vector3 bodyRight = GetWorldVectorFromLocalVector(rotation, LOCAL_RIGHT);

    float dotRight = Vector3DotProduct(LOCAL_UP, bodyRight);

    ApplyTorqueLocal(body, LOCAL_UP, BANK * -dotRight);

    //upside down case

    Vector3 bodyUp = GetWorldVectorFromLocalVector(rotation, LOCAL_UP);

    float dotUp = Vector3DotProduct(LOCAL_UP, bodyUp);

    if(dotUp <= -0.1f) ApplyTorqueLocal(body, LOCAL_RIGHT, BANK_PITCH * dotUp);

    //fake stall

    Vector3 axisOfRotation = Vector3CrossProduct(bodyForward, LOCAL_DOWN);

    axisOfRotation = Vector3Normalize(axisOfRotation);

    float dotForwardDown = Vector3DotProduct(LOCAL_DOWN, bodyForward);

    float stallDot = 0.6f;

    if((forwardSpeed <= config.stallSpeed) && (dotForwardDown < stallDot))
    {
        player.pitchUp = false;
        player.pitchDown = false;

        player.rollLeft = false;
        player.rollRight = false;

        player.yawLeft = false;
        player.yawRight = false;

        ApplyTorque(body, axisOfRotation, STALL_FORCE);
    }
}

void UpdateCameraTransform(const Transform &targerTransform, float dt)
{
    Vector3 rotatedOffset = Vector3RotateByQuaternion(cameraOffset, targerTransform.rotation);

    Vector3 targetForward = GetWorldVectorFromLocalVector(targerTransform.rotation, LOCAL_FORWARD);

    camera.up = GetWorldVectorFromLocalVector(targerTransform.rotation, LOCAL_UP);
    
    camera.target = targerTransform.translation + targetForward;

    camera.position = targerTransform.translation + rotatedOffset;
}
