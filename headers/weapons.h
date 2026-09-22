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

void UpdateBulletPool(BulletPool& bulletpool,float dt);

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

        Vector3 bulletSpawnPos = position + Vector3RotateByQuaternion(offset, QuaternionNormalize(rotation));
        
        SpawnBullet(bulletpool, bulletSpawnPos, initialVel, rotation);

        bulletpool.fireTimer = bulletpool.properties.firerate;
    }
}

//Missiles / Bombs



//Effects