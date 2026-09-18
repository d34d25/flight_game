#include "player.h"

void UpdatePlayerInput(Player &player)
{
    player.throttleUp = IsKeyDown(KEY_W);
    player.throttleDown = IsKeyDown(KEY_S);

    if(player.stalling)
    {
        RemoveControlPlayer(player);

        return;
    }

    player.pitchUp = IsKeyDown(KEY_DOWN);
    player.pitchDown = IsKeyDown(KEY_UP);

    player.rollLeft = IsKeyDown(KEY_LEFT);
    player.rollRight = IsKeyDown(KEY_RIGHT);

    player.yawLeft = IsKeyDown(KEY_A);
    player.yawRight = IsKeyDown(KEY_D);
}

void UpdatePlayer(Player &player, float dt)
{
    Body& body = player.aircraft.body;

    const AircraftConfig& config = GetAircraftConfig(player.aircraft);

    float& thrust = player.aircraft.thrust;

    const Quaternion& rotation = body.transform.rotation;

    Vector3 bodyForward = GetWorldVectorFromLocalVector(rotation, LOCAL_FORWARD);

    float forwardSpeed = Vector3DotProduct(body.linearVelocity, bodyForward);

    float mobilityFactor = CalculateMobilityFactor(forwardSpeed, config);

    if(player.pitchDown) ApplyTorqueLocal(body, LOCAL_RIGHT, config.pitch * mobilityFactor);
    else if(player.pitchUp) ApplyTorqueLocal(body, LOCAL_RIGHT, -config.pitch * mobilityFactor);

    if(player.rollLeft) ApplyTorqueLocal(body, LOCAL_FORWARD, -config.roll * mobilityFactor);
    else if(player.rollRight) ApplyTorqueLocal(body, LOCAL_FORWARD, config.roll * mobilityFactor);

    if(player.yawRight) ApplyTorqueLocal(body, LOCAL_UP, -config.yaw * mobilityFactor);
    else if(player.yawLeft) ApplyTorqueLocal(body, LOCAL_UP, config.yaw * mobilityFactor);

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

    //drag / frake gravity

    float dotForwardUp = Vector3DotProduct(bodyForward, LOCAL_UP);

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

    if(forwardSpeed <= config.stallSpeed) player.stalling = true;
    else if (forwardSpeed > config.recoverySpeed) player.stalling = false;
    
    if(player.stalling && dotForwardDown < stallDot) ApplyTorque(body, axisOfRotation, STALL_FORCE);
}

void UpdateCameraTransform(const Player& player, const Transform &targerTransform, float dt)
{
    const Body& body = player.aircraft.body;

    const AircraftConfig& config = GetAircraftConfig(player.aircraft);

    const Quaternion& rotation = body.transform.rotation;

    Vector3 bodyForward = GetWorldVectorFromLocalVector(rotation, LOCAL_FORWARD);

    float forwardSpeed = Vector3DotProduct(player.aircraft.body.linearVelocity, bodyForward);

    Vector3 rotatedOffset = Vector3RotateByQuaternion(cameraOffset, targerTransform.rotation);

    Vector3 targetForward = GetWorldVectorFromLocalVector(targerTransform.rotation, LOCAL_FORWARD);

    float alpha = 5.0f;

    float t = alpha * dt;

    lastFrameCameraOffset += Vector3Scale(Vector3Subtract(rotatedOffset, lastFrameCameraOffset), t);

    camera.up = GetWorldVectorFromLocalVector(targerTransform.rotation, LOCAL_UP);
    
    camera.target = targerTransform.translation + targetForward * 200.0f;

    camera.position = targerTransform.translation + lastFrameCameraOffset;

    camera.fovy = BASE_FOVY * CalculateCameraFOVFactor(forwardSpeed, config);
}
