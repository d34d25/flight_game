#pragma once

#include "aircraft.h"

constexpr float MAX_DIST_TO_PLAYER_SQR = 600 * 600;

enum struct EnemyType
{
    FIGHTER,
    CHOPPER,

    //sams will just be "aircrafts" that cannot fly or move
    SAM,
    AA_GUN
};

enum struct EnemyState
{
    APPROACHING, //approaches to the player if they are too far appart
    
    ENGAGING, //tries to be and stay in player's six
    TAKING_DISTANCE, //takes distance for the player in order to re-engage

    NON_AIRCRAFT //default state for SAMs and AA Guns
};

struct Enemy
{
    Aircraft aircraft;

    EnemyType type;

    EnemyState currentState;

    bool isBoss;
};

void UpdateEnemy(Enemy& enemy, Vector3 playerPos);

void UpdateEnemyAI(Enemy& enemy, Vector3 playerPos); //will run at a lower framerate

void RotateTowardsPointPitchYaw(Enemy& enemy, Vector3 target); //rotates while keeping the enemy's local up towards the target (affects roll and pitch)

inline void InitEnemy(Enemy& enemy, Vector3 playerPos)
{
    Vector3& enemyPos = enemy.aircraft.body.transform.translation;
    Quaternion& enemyRotation = enemy.aircraft.body.transform.rotation;

    switch (enemy.type)
    {
    case EnemyType::FIGHTER:
    {
        enemy.aircraft = InitAircraft(SUTYPE);
        
        AircraftType type = enemy.aircraft.type;

        enemyPos = {
            0.0f, 800.0f, 600.0f
        };

        Vector3 directionToPlayer = playerPos - enemyPos;

        directionToPlayer = Vector3Normalize(directionToPlayer);

        Vector3 worldUp = LOCAL_UP;

        if(fabs(Vector3DotProduct(directionToPlayer, LOCAL_UP) > 0.999f))
        {
            worldUp = LOCAL_DOWN;
        }

        Vector3 right = Vector3Normalize(Vector3CrossProduct(worldUp, directionToPlayer));

        Vector3 up = Vector3CrossProduct(directionToPlayer, right);

        Matrix matrixRotation = {
            right.x, up.x, directionToPlayer.x, 0.0f,
            right.y, up.y, directionToPlayer.y, 0.0f,
            right.z, up.z, directionToPlayer.z, 0.0f,
            0.0f,    0.0f, 0.0f,                1.0f
        };

        enemyRotation = QuaternionFromMatrix(matrixRotation);

        Vector3 bodyDirection = GetWorldVectorFromLocalVector(enemyRotation, LOCAL_FORWARD);

        enemy.aircraft.body.linearVelocity = Vector3Normalize(bodyDirection) * aircraftsDB[type].idleSpeed;

        enemy.aircraft.thrust = aircraftsDB[type].idleThrust;
    }
    break;

    case EnemyType::CHOPPER: 
    {
        //not implemented yet
    }
    break;

    case EnemyType::SAM: 
    {
        //not implemented yet
    }
    break;

    case EnemyType::AA_GUN: 
    {
        //not implemented yet
    }
    break;
    
    default:
        break;
    }
};