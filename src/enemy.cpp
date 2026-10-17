#include "enemy.h"

void UpdateEnemy(Enemy &enemy, Vector3 playerPos)
{
    switch (enemy.currentState)
    {
    case EnemyState::APPROACHING:
    {

    }
    break;
    
    case EnemyState::ENGAGING:
    {
        RotateTowardsPointPitchYaw(enemy, playerPos);
    }
    break;

    case EnemyState::TAKING_DISTANCE:
    {

    }
    break;

    case EnemyState::NON_AIRCRAFT:
    {

    }
    break;


    default:
        break;
    }

    AircraftConfig& config = aircraftsDB[enemy.aircraft.type];

    ApplyForceLocal(enemy.aircraft.body, LOCAL_FORWARD, enemy.aircraft.thrust);
}

void UpdateEnemyAI(Enemy &enemy, Vector3 playerPos)
{
    Vector3& enemyPos = enemy.aircraft.body.transform.translation;
    Quaternion& enemyRotation = enemy.aircraft.body.transform.rotation;

    Vector3 enemyPlayerPosDiff = playerPos - enemyPos;

    Vector3 directionToPlayer = Vector3Normalize(enemyPlayerPosDiff);

    float distanceToPlayerSqr = Vector3LengthSqr(enemyPlayerPosDiff);

    if(distanceToPlayerSqr >= MAX_DIST_TO_PLAYER_SQR)
    {
        //enemy.currentState = EnemyState::APPROACHING;
        enemy.currentState = EnemyState::ENGAGING;
    }
    else
    {
        enemy.currentState = EnemyState::ENGAGING;
    }

    std::cout<<"enemy state: ";

    switch (enemy.currentState)
    {
    case EnemyState::APPROACHING:
    {
        std::cout<<"APPROACHING\n";
    }
    break;
    
    case EnemyState::ENGAGING:
    {
        std::cout<<"ENGAGING\n";
    }
    break;

    case EnemyState::TAKING_DISTANCE:
    {
        std::cout<<"TAKING_DISTANCE\n";
    }
    break;

    case EnemyState::NON_AIRCRAFT:
    {
        std::cout<<"NON_AIRCRAFT\n";
    }
    break;


    default:
        break;
    }
}

//rotates while keeping the enemy's local up towards the target (affects roll and pitch)
void RotateTowardsPointPitchYaw(Enemy &enemy, Vector3 target)
{
    AircraftConfig& config = aircraftsDB[enemy.aircraft.type];

    Body& enemyBody = enemy.aircraft.body;

    Vector3& enemyPos = enemyBody.transform.translation;
    Quaternion& enemyRotation = enemyBody.transform.rotation;

    Vector3 toTarget = Vector3Normalize(target - enemyPos);

    Vector3 bodyForward = GetWorldVectorFromLocalVector(enemyRotation, LOCAL_FORWARD);
    Vector3 bodyUp = GetWorldVectorFromLocalVector(enemyRotation, LOCAL_UP);
    Vector3 bodyRight = GetWorldVectorFromLocalVector(enemyRotation, LOCAL_RIGHT);

    Vector3 forwardAxisError = Vector3CrossProduct(bodyForward, toTarget);

    float pitchError = Vector3DotProduct(forwardAxisError, bodyRight);

    Vector3 targetOnPlane = toTarget - bodyForward * Vector3DotProduct(toTarget, bodyForward);

    float rollError = Vector3DotProduct(Vector3CrossProduct(bodyUp, targetOnPlane), bodyForward);

    float errorThreshold = 0.1f;

    if(pitchError > errorThreshold)
    {
        ApplyTorqueLocal(enemyBody, LOCAL_RIGHT, config.pitch);
    }
    else if(pitchError < -errorThreshold)
    {
        ApplyTorqueLocal(enemyBody, LOCAL_RIGHT, -config.pitch);
    }

    if(rollError > errorThreshold)
    {
        ApplyTorqueLocal(enemyBody, LOCAL_FORWARD, config.roll);
    }
    else if(rollError < -errorThreshold)
    {
        ApplyTorqueLocal(enemyBody, LOCAL_FORWARD, -config.roll);
    }
}
