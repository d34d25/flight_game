#include "player.h"

void UpdatePlayerInput(Player &player)
{
    player.throttleUp = IsKeyDown(KEY_W);
    player.throttleDown = IsKeyDown(KEY_S);

    player.firingBullet = IsKeyDown(KEY_LEFT_SHIFT);

    if(IsKeyPressed(KEY_SPACE)) player.firingMsl = true;

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

    BulletPool& bulletpool = player.aircraft.bulletpool;

    FireBullet(
        bulletpool,
        bodyForward,
        rotation,
        body.transform.translation,
        config.gunOffset,
        body.linearVelocity,
        dt,
        player.firingBullet
    );

    UpdateBulletPool(bulletpool, dt);

    float forwardSpeed = Vector3DotProduct(body.linearVelocity, bodyForward);

    float mobilityFactor = CalculateMobilityFactor(forwardSpeed, config);

    if(player.pitchDown) ApplyTorqueLocal(body, LOCAL_RIGHT, config.pitch * mobilityFactor);
    else if(player.pitchUp) ApplyTorqueLocal(body, LOCAL_RIGHT, -config.pitch * mobilityFactor);

    if(player.rollLeft) ApplyTorqueLocal(body, LOCAL_FORWARD, -config.roll * mobilityFactor);
    else if(player.rollRight) ApplyTorqueLocal(body, LOCAL_FORWARD, config.roll * mobilityFactor);

    if(player.yawRight) ApplyTorqueLocal(body, LOCAL_UP, -config.yaw * mobilityFactor);
    else if(player.yawLeft) ApplyTorqueLocal(body, LOCAL_UP, config.yaw * mobilityFactor);

    float engineGlowChangeRate = 0.25f;
    float idleEngineGlow = 0.5f;

    if(player.throttleUp)
    {
        thrust += config.acceleration * dt;

        player.engineGlow += engineGlowChangeRate * dt;

        if(thrust >= config.maxThrust) thrust = config.maxThrust;
    }
    else if(player.throttleDown)
    {
        thrust -= config.breakPower * dt;

        player.engineGlow -= engineGlowChangeRate * dt;

        if(thrust <= 0.0f) thrust = 0.0f;
    }
    else
    {
        if(thrust < config.idleThrust)
        {
            thrust += config.recoveryBelowIdle * dt;

            player.engineGlow += engineGlowChangeRate * dt;

            if(player.engineGlow >= idleEngineGlow) player.engineGlow = idleEngineGlow;

            if(thrust >= config.idleThrust) thrust = config.idleThrust;
        }
        else
        {
            thrust -= config.recoveryAboveIdle * dt;

            player.engineGlow -= engineGlowChangeRate * dt;

            if(player.engineGlow <= idleEngineGlow) player.engineGlow = idleEngineGlow;

            if(thrust <= config.idleThrust) thrust = config.idleThrust;
        }
    }

    player.engineGlow = Clamp(player.engineGlow, 0.0f, 1.0f);

    SetShaderValue(player.engineShader, player.engineBrightnessLoc, &player.engineGlow, SHADER_UNIFORM_FLOAT);

    Vector3 mslOffset = config.mslOffset;

    switch (player.aircraft.mslFired)
    {
    case 1: mslOffset.x = -config.mslOffset.x; break;
    
    default: break;
    }

    bool firedMissile = FireMissile(
            player.aircraft.missilePool,
            rotation,
            body.transform.translation,
            mslOffset,
            thrust,
            body.forwardDrag,
            dt,
            player.firingMsl
    );

    if(player.firingMsl) player.firingMsl = false;

    if(firedMissile)
    {
        player.aircraft.mslFired++;

        if(player.aircraft.mslFired >= player.aircraft.hardpoints)
        {
            player.aircraft.mslFired = 0;
            player.aircraft.missilePool.fireTimer = player.aircraft.missilePool.properties.firerate;
        }
    }

    UpdateMissilePool(player.aircraft.missilePool, dt);

    ApplyForceLocal(body, LOCAL_FORWARD, thrust);

    //plane physics

    //drag / frake gravity

    float dotForwardUp = Vector3DotProduct(bodyForward, LOCAL_UP);

    float upDragTolerance = 0.1f;

    if(dotForwardUp > upDragTolerance)
    {
        if(forwardSpeed > 0.0f) ApplyForceLocal(body, LOCAL_BACKWARD, config.gravity * dotForwardUp);
    }
    else if(dotForwardUp < -upDragTolerance)
    {
        ApplyForceLocal(body, LOCAL_BACKWARD, config.gravity * dotForwardUp);
    }

    //fake banking

    float bankFactor = CalculateBankFactor(forwardSpeed, config);

    Vector3 bodyRight = GetWorldVectorFromLocalVector(rotation, LOCAL_RIGHT);

    float dotRight = Vector3DotProduct(LOCAL_UP, bodyRight);

    ApplyTorqueLocal(body, LOCAL_UP, config.bank * -dotRight * bankFactor);

    //upside down case

    Vector3 bodyUp = GetWorldVectorFromLocalVector(rotation, LOCAL_UP);

    float dotUp = Vector3DotProduct(LOCAL_UP, bodyUp);

    if(dotUp <= -0.1f) ApplyTorqueLocal(body, LOCAL_RIGHT, config.bankPitch * dotUp * bankFactor);

    //fake stall

    Vector3 axisOfRotation = Vector3CrossProduct(bodyForward, LOCAL_DOWN);

    axisOfRotation = Vector3Normalize(axisOfRotation);

    float stallDrag = fabs(axisOfRotation.x) * config.angularDrag.x +
    fabs(axisOfRotation.y) * config.angularDrag.y + fabs(axisOfRotation.z) * config.angularDrag.z;

    float stallTorque = GetDesiredValue(STALL_TORQUE, stallDrag);

    float dotForwardDown = Vector3DotProduct(LOCAL_DOWN, bodyForward);

    float stallDot = 0.6f;

    if(forwardSpeed <= config.stallSpeed) player.stalling = true;
    else if (forwardSpeed > config.recoverySpeed) player.stalling = false;
    
    if(player.stalling && dotForwardDown < stallDot) ApplyTorque(body, axisOfRotation, stallTorque);
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
