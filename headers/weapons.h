#pragma once

#include <memory>

#include <vector>

#include "body.h"

//Bullets

struct BulletPoolProperties
{
    float lifeTime;

    float spread;

    float speed;

    float firerate;

    float bulletSize;
};

struct Bullet
{
    Transform transform;

    Vector3 velocity;

    float lifeTime;

    float bulletSize;

    float currentTime;

    bool didhHit;
};

inline void UpdateBullet(Bullet* bullet, float dt)
{
    bullet->transform.translation += bullet->velocity * dt;
}

struct BulletPool
{
    std::vector<std::unique_ptr<Bullet>> bullets;

    std::vector<Bullet*> activeBullets;

    std::vector<Bullet*> inactiveBullets;

    BulletPoolProperties properties;

    float fireTimer;
};

void InitBulletPool(BulletPool& bulletpool, BulletPoolProperties properties, int quantity);

void UpdateBulletPool(BulletPool& bulletpool, float dt);

void SpawnBullet(BulletPool& bulletpool, Vector3 position, Vector3 initalVelocity, Quaternion rotation = QuaternionIdentity());

inline void UpdateBulletPoolProperties(BulletPool& bulletpool, BulletPoolProperties properties)
{
    bulletpool.properties = properties;
}

enum BulletType
{
    VULKAN,
    GUN_COUNT
};

inline BulletPoolProperties gunsDB[GUN_COUNT];

void InitGunDB();

inline void FireBullet(
    BulletPool& bulletpool,
    Vector3 direction,
    Quaternion rotation,
    Vector3 position,
    Vector3 offset,
    Vector3 linearVelocity,
    float dt,
    bool condition = true
)
{
    if(bulletpool.fireTimer > 0.0f) bulletpool.fireTimer -= dt;

    if(condition && bulletpool.fireTimer <= 0.0f)
    {
        Vector3 finalDirection = direction;

        if(bulletpool.properties.spread > 0.0f)
        {
            Vector3 right = Vector3RotateByQuaternion(LOCAL_RIGHT, rotation);
            Vector3 up = Vector3RotateByQuaternion(LOCAL_UP, rotation);

            float rx = ((float)GetRandomValue(-100,100) / 100.0f) * bulletpool.properties.spread;
            float ry = ((float)GetRandomValue(-100,100) / 100.0f) * bulletpool.properties.spread;

            finalDirection.x += (right.x * rx) + (up.x * ry);
            finalDirection.y += (right.y * rx) + (up.y * ry);
            finalDirection.z += (right.z * rx) + (up.z * ry);

            finalDirection = Vector3Normalize(finalDirection);
        }

        Vector3 initialVel = (finalDirection * bulletpool.properties.speed) + linearVelocity;

        Quaternion normRotation = QuaternionNormalize(rotation);

        Vector3 bulletSpawnPos = position + Vector3RotateByQuaternion(offset, normRotation);
        
        SpawnBullet(bulletpool, bulletSpawnPos, initialVel, normRotation);

        bulletpool.fireTimer = bulletpool.properties.firerate;
    }
}

//Missiles / Bombs

constexpr float MSL_ANGULAR_DRAG = 2.0f;

struct MissileProperties
{
    float lifeTime;

    float maxSpeed;

    float acceleration;

    float missileSize;

    float firerate;
};

struct Missile
{
    Body body;

    float lifeTime;

    float missileSize;

    float currentTime;

    float maxSpeed;

    float maxThrust;

    float thrust;

    float acceleration;

    bool didHit;
};

void UpdateMissile(Missile* missile, float dt);

struct MissilePool
{
    std::vector<std::unique_ptr<Missile>> missiles;

    std::vector<Missile*> activeMissiles;

    std::vector<Missile*> inactiveMissiles;

    MissileProperties properties;

    float fireTimer;
};

void InitMissilePool(MissilePool& missilePool, MissileProperties properties, int quantity);

void UpdateMissilePool(MissilePool& missilePool, float dt);

void SpawnMissile(MissilePool& missilePool, Vector3 position, Vector3 initialVelocity, float thrust, float drag, Quaternion rotation = QuaternionIdentity());

enum MissileType
{
    STANDARD_MSL,
    MISSILE_COUNT
};

inline MissileProperties missilesDB[MISSILE_COUNT];

void InitMissileDB();

inline void FireMissile(
    MissilePool& missilepool,
    Quaternion rotation,
    Vector3 position,
    Vector3 offset,
    Vector3 linearVelocity,
    float thrust,
    float drag,
    float dt,
    bool condition = true
)
{
    if(missilepool.fireTimer > 0.0f) missilepool.fireTimer -= dt;

    if(condition && missilepool.fireTimer <= 0.0f)
    {
        Quaternion normRotation = QuaternionNormalize(rotation);

        Vector3 mslSpawnPos = position + Vector3RotateByQuaternion(offset, normRotation);

        SpawnMissile(missilepool, mslSpawnPos, linearVelocity, thrust, drag, normRotation);
        
        missilepool.fireTimer = missilepool.properties.firerate;
    }
}


//Effects