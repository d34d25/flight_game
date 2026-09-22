#include "weapons.h"

//guns

void InitBulletPool(BulletPool& bulletpool, BulletPoolProperties properties, int quantity)
{
    bulletpool.properties = properties;

    bulletpool.fireTimer = properties.firerate;

    for(int i = 0; i < quantity; i++)
    {
        std::unique_ptr<Bullet> tempBullet = std::make_unique<Bullet>();

        tempBullet->currentTime = 0.0f;
        tempBullet->didhHit = false;

        tempBullet->bulletSize = properties.bulletSize;
        tempBullet->lifeTime = properties.lifeTime;

        bulletpool.bullets.push_back(std::move(tempBullet));
    }

    for(const auto& bulletptr : bulletpool.bullets)
    {
        bulletpool.inactiveBullets.push_back(bulletptr.get());
    }
}

void UpdateBulletPool(BulletPool& bulletpool, float dt)
{
    for(int i = 0; i < bulletpool.activeBullets.size();)
    {
        Bullet* b = bulletpool.activeBullets[i];

        if(b)
        {
            UpdateBullet(b, dt);

            b->currentTime += dt;

            if(b->currentTime >= b->lifeTime || b->didhHit)
            {
                bulletpool.inactiveBullets.push_back(b);

                bulletpool.activeBullets[i] = bulletpool.activeBullets.back();
                bulletpool.activeBullets.pop_back();
            }
            else
            {
                i++;
            }
        }
        else
        {
            i++;
        }
    }
}

void SpawnBullet(BulletPool& bulletpool, Vector3 position, Vector3 initalVelocity, Quaternion rotation)
{
    if(!bulletpool.inactiveBullets.empty())
    {
        Bullet* b = bulletpool.inactiveBullets.back();
        bulletpool.inactiveBullets.pop_back();

        b->transform.translation = position;
        b->transform.rotation = rotation;

        b->velocity = initalVelocity;

        b->didhHit = false;
        b->currentTime = 0.0f;

        b->bulletSize = bulletpool.properties.bulletSize;
        b->lifeTime = bulletpool.properties.lifeTime;

        bulletpool.activeBullets.push_back(b);
    }
}

void InitGunDB()
{
    BulletPoolProperties& vulkan = gunsDB[VULKAN];

    vulkan.bulletSize = 0.15f;
    vulkan.firerate = 0.1f;
    vulkan.lifeTime = 2.0f;
    vulkan.speed = 400.0f;
    vulkan.spread = 0.0025f;
}

//missiles

void UpdateMissile(Missile *missile, float dt)
{
    missile->thrust += missile->acceleration * dt;

    if(missile->thrust >= missile->maxThrust) missile->thrust = missile->maxThrust;

    ApplyForceLocal(missile->body, LOCAL_FORWARD, missile->thrust);

    UpdateBody(missile->body, dt);
}

void InitMissilePool(MissilePool &missilePool, MissileProperties properties, int quantity)
{
    missilePool.properties = properties;

    missilePool.fireTimer = properties.firerate;

    for(int i = 0; i < quantity; i++)
    {
        std::unique_ptr<Missile> tempMissile = std::make_unique<Missile>();

        tempMissile->currentTime = 0.0f;
        tempMissile-> didHit = false;

        tempMissile->missileSize = properties.missileSize;
        tempMissile->lifeTime = properties.lifeTime;

        tempMissile->maxSpeed = properties.maxSpeed;
        tempMissile->maxThrust = 1.0f;

        tempMissile->acceleration = properties.acceleration;

        tempMissile->body.angularDrag = {MSL_ANGULAR_DRAG,MSL_ANGULAR_DRAG,MSL_ANGULAR_DRAG};

        missilePool.missiles.push_back(std::move(tempMissile));
    }

    for(const auto& missilePtr : missilePool.missiles)
    {
        missilePool.inactiveMissiles.push_back(missilePtr.get());
    }
}

void UpdateMissilePool(MissilePool &missilePool, float dt)
{
    for(int i = 0; i < missilePool.activeMissiles.size();)
    {
        Missile* m = missilePool.activeMissiles[i];

        if(m)
        {
            UpdateMissile(m, dt);

            m->currentTime += dt;

            if(m->currentTime >= m->lifeTime || m->didHit)
            {
                missilePool.inactiveMissiles.push_back(m);

                missilePool.activeMissiles[i] = missilePool.activeMissiles.back();
                missilePool.activeMissiles.pop_back();
            }
            else
            {
                i++;
            }
        }
        else
        {
            i++;
        }
    }
}

void SpawnMissile(MissilePool &missilePool, Vector3 position, Vector3 initialVelocity, float thrust, float drag, Quaternion rotation)
{
    if(!missilePool.inactiveMissiles.empty())
    {
        Missile* m = missilePool.inactiveMissiles.back();
        missilePool.inactiveMissiles.pop_back();

        m->body.transform.translation = position;
        m->body.transform.rotation = rotation;

        //drag and forward forces calculated here to match the drag of the shooter
        m->body.forwardDrag = drag;

        m->acceleration = GetDesiredValue(missilePool.properties.acceleration, drag);
        m->maxThrust = GetDesiredValue(missilePool.properties.maxSpeed, drag);

        m->body.linearVelocity = initialVelocity;
        m->thrust = thrust;

        m->didHit = false;
        m->currentTime = 0.0f;

        m->missileSize = missilePool.properties.missileSize;
        m->lifeTime = missilePool.properties.lifeTime;

        missilePool.activeMissiles.push_back(m);
    }
}

void InitMissileDB()
{
    MissileProperties& standardMsl = missilesDB[STANDARD_MSL];

    standardMsl.missileSize = 0.5f;
    standardMsl.firerate = 1.0f;
    standardMsl.lifeTime = 4.0f;

    standardMsl.maxSpeed = 1200.0f;

    standardMsl.acceleration = 3000.0f;
}
