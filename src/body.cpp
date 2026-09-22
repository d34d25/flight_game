#include "body.h"

void UpdateBody(Body &body, float dt)
{
    Vector3& translation = body.transform.translation;

    Quaternion& rotation = body.transform.rotation;

    //calculate world space movement direction

    //where the nose is poitning in world space
    Vector3 bodyForward = GetWorldVectorFromLocalVector(rotation, LOCAL_FORWARD);

    //how fast it is moving at that direction
    float bodyForwardSpeed = Vector3DotProduct(bodyForward, body.linearVelocity);

    Vector3 bodyForwardSpeedVec = bodyForward * bodyForwardSpeed;

    //non-forward velocity
    Vector3 bodySideVel = Vector3Subtract(body.linearVelocity, bodyForwardSpeedVec);

    //calculate maximum drag
    float maxDrag = MASS / dt;

    float forwardDrag = body.forwardDrag;

    float sideDrag = body.sideDrag;

    if(forwardDrag >= maxDrag) forwardDrag = maxDrag;
    
    if(sideDrag >= maxDrag) sideDrag = maxDrag;

    //apply forward drag

    Vector3 forwardDragForce = bodyForwardSpeedVec * -forwardDrag;

    body.force += forwardDragForce;

    //apply non forward drag

    Vector3 sideDragForce = bodySideVel * -sideDrag;

    body.force += sideDragForce;

    //linear update (world space)

    Vector3 linearAcceleration = {0.0f,0.0f,0.0f};

    linearAcceleration = body.force / MASS;

    body.linearVelocity += linearAcceleration * dt;

    body.transform.translation += body.linearVelocity * dt;

    body.force = {0.0f,0.0f,0.0f};

    //angular update (world space)

    float maxAngularDrag = INERTIA / dt;

    Vector3 bodyPitch = GetWorldVectorFromLocalVector(rotation, LOCAL_RIGHT);

    Vector3 bodyRoll = bodyForward;

    Vector3 bodyYaw = GetWorldVectorFromLocalVector(rotation, LOCAL_UP);

    float pitchDrag = body.angularDrag.x;
    float rollDrag = body.angularDrag.z;
    float yawDrag = body.angularDrag.y;

    if(pitchDrag > maxAngularDrag) pitchDrag = maxAngularDrag;
    if(rollDrag > maxAngularDrag) rollDrag = maxAngularDrag;
    if(yawDrag > maxAngularDrag) yawDrag = maxAngularDrag;

    float pitchSpeed = Vector3DotProduct(body.angularVelocity, bodyPitch);
    float rollSpeed = Vector3DotProduct(body.angularVelocity, bodyRoll);
    float yawSpeed = Vector3DotProduct(body.angularVelocity, bodyYaw);

    body.torque += bodyPitch * (pitchSpeed * -pitchDrag);
    body.torque += bodyRoll * (rollSpeed * -rollDrag);
    body.torque += bodyYaw * (yawSpeed * -yawDrag);

    Vector3 angularAcceleration = {0.0f,0.0f,0.0f};

    angularAcceleration = body.torque / INERTIA;

    body.angularVelocity += angularAcceleration * dt;

    float angularVelocityMagnitude = Vector3Length(body.angularVelocity);

    if(angularVelocityMagnitude > 0.0f)
    {
        Vector3 axis = body.angularVelocity / angularVelocityMagnitude;

        float angle = angularVelocityMagnitude * dt;

        Quaternion deltaQ = QuaternionFromAxisAngle(axis, angle);

        body.transform.rotation = QuaternionNormalize(QuaternionMultiply(deltaQ, rotation));
    }

    body.torque = {0.0f,0.0f,0.0f};
}
