#include "weapons.h"

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
