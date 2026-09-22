#include "weapons.h"

//guns

void InitBulletPool(BulletPool& bulletpool, BulletPoolProperties properties, int quantity)
{
    bulletpool.properties = properties;

    bulletpool.fireTimer = 0.0f;

    for(int i = 0; i < quantity; i++)
    {
        std::unique_ptr<Bullet> tempBullet = std::make_unique<Bullet>();

        tempBullet->currentTime = 0.0f;
        tempBullet->didhHit = false;

        tempBullet->properties = properties;

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

            if(b->currentTime >= b->properties.lifeTime || b->didhHit)
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

        b->properties.bulletSize = bulletpool.properties.bulletSize;
        b->properties.lifeTime = bulletpool.properties.lifeTime;

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

void InitTrailDB()
{
    TrailProperties& mslTrail = trailsDB[MSL_TRAIL];

    mslTrail.firerate = 0.025f;
    mslTrail.lifeTime = 2.0f;
    mslTrail.radius = 0.5f;
    mslTrail.radiusIncreaseRate = 0.1f;

    TrailProperties& explosion = trailsDB[EXPLOSION];

    explosion.firerate = 0.000001f;
    explosion.lifeTime = 3.0f;
    explosion.radius = 10.0f;
    explosion.radiusIncreaseRate = 0.4f;
}

void InitTrailPool(TrailPool &trailpool, TrailProperties properties, int quantity)
{
    trailpool.properties = properties;

    trailpool.fireTimer = 0.0f;

    for(int i = 0; i < quantity; i++)
    {
        std::unique_ptr<Trail> tempTrail = std::make_unique<Trail>();

        tempTrail->properties = properties;

        tempTrail->currentTime = 0.0f;

        trailpool.trails.push_back(std::move(tempTrail));
    }

    for(const auto& trailPtr : trailpool.trails)
    {
        trailpool.inactiveTrails.push_back(trailPtr.get());
    }
}

void UpdateTrailPool(TrailPool &trailpool, float dt)
{
    for(int i = 0; i < trailpool.activeTrails.size();)
    {
        Trail* t = trailpool.activeTrails[i];

        if(!t)
        {
            UpdateTrail(t, dt);

            t->currentTime += dt;

            if(t-> currentTime >= t->properties.lifeTime)
            {
                trailpool.inactiveTrails.push_back(t);

                trailpool.activeTrails[i] = trailpool.activeTrails.back();
                trailpool.activeTrails.pop_back();
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

void SpawnTrail(TrailPool &trailpool, Vector3 position, Vector3 velocity)
{
    if(!trailpool.inactiveTrails.empty())
    {
        Trail* t = trailpool.inactiveTrails.back();
        trailpool.inactiveTrails.pop_back();

        t->position = position;
        t->velocity = velocity;

        t->currentTime = 0.0f;

        trailpool.activeTrails.push_back(t);
    }
}

inline void ResetTrailPool(TrailPool& trailpool)
{
    for(int i = 0; i < trailpool.activeTrails.size(); i++)
    {
        Trail* t = trailpool.activeTrails[i];

        if(t)
        {
            t->currentTime = t->properties.lifeTime;
            
            trailpool.inactiveTrails.push_back(t);
        }
    }

    trailpool.activeTrails.clear();
}

//missiles

void UpdateMissile(Missile *missile, float dt)
{
    missile->thrust = missile->maxThrust;

    ApplyForceLocal(missile->body, LOCAL_FORWARD, missile->thrust);

    UpdateBody(missile->body, dt);
}

void InitMissilePool(MissilePool &missilePool, MissileProperties properties, int quantity)
{
    missilePool.properties = properties;

    missilePool.fireTimer = 0.0f;

    for(int i = 0; i < quantity; i++)
    {
        std::unique_ptr<Missile> tempMissile = std::make_unique<Missile>();

        InitTrailPool(tempMissile->trailPool, trailsDB[MSL_TRAIL], 150);

        tempMissile->currentTime = 0.0f;
        tempMissile-> didHit = false;

        tempMissile-> properties = properties;

        tempMissile->maxThrust = 1.0f;

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
    if(missilePool.fireTimer > 0.0f) missilePool.fireTimer -= dt;
    
    for(int i = 0; i < missilePool.activeMissiles.size();)
    {
        Missile* m = missilePool.activeMissiles[i];

        if(m)
        {
            UpdateMissile(m, dt);

            m->currentTime += dt;

            FireTrail(
                m->trailPool,
                m->body.transform.translation,
                GetWorldVectorFromLocalVector(m->body.transform.rotation, LOCAL_BACKWARD) * 10.0f,
                dt,
                true
            );

            UpdateTrailPool(m->trailPool, dt);

            if(m->currentTime >= m->properties.lifeTime || m->didHit)
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

void SpawnMissile(MissilePool &missilePool, Vector3 position, float thrust, float drag, Quaternion rotation)
{
    if(!missilePool.inactiveMissiles.empty())
    {
        Missile* m = missilePool.inactiveMissiles.back();
        missilePool.inactiveMissiles.pop_back();

        ResetTrailPool(m->trailPool);

        m->body.transform.translation = position;
        m->body.transform.rotation = rotation;

        //drag and forward forces calculated here to match the drag of the shooter
        m->body.forwardDrag = drag;
        m->maxThrust = GetDesiredValue(missilePool.properties.maxSpeed, drag);

        Vector3 direction = GetWorldVectorFromLocalVector(rotation, LOCAL_FORWARD);

        m->body.linearVelocity = direction * missilePool.properties.maxSpeed;
        m->thrust = thrust;

        m->didHit = false;
        m->currentTime = 0.0f;

        m->properties.missileSize = missilePool.properties.missileSize;
        m->properties.lifeTime = missilePool.properties.lifeTime;

        missilePool.activeMissiles.push_back(m);
    }
}

void InitMissileDB()
{
    MissileProperties& standardMsl = missilesDB[STANDARD_MSL];

    standardMsl.missileSize = 0.5f;
    standardMsl.firerate = 0.75f;
    standardMsl.lifeTime = 4.0f;

    standardMsl.maxSpeed = 200.0f;
}