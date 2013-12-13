#ifndef POWERUP_CARRIER_H
#define POWERUP_CARRIER_H

#include "GameEntity.h"
#include "Collisionable.h"

class PowerupCarrier: public GameEntity, public Collisionable
{
private:
    float speed;
public:
    PowerupCarrier();
    virtual ~PowerupCarrier();
    virtual void update(float delta);

    virtual bool takeDamage(sf::Vector2f position, int damageType, int damageAmount);
    virtual void render(sf::RenderTarget& window);
};

#endif//POWERUP_CARRIER_H
