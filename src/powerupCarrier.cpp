#include <math.h>

#include "powerup.h"
#include "powerupCarrier.h"
#include "explosion.h"
#include "textureManager.h"

#include "scriptInterface.h"
REGISTER_SCRIPT_CLASS(PowerupCarrier)
{
    REGISTER_SCRIPT_CLASS_FUNCTION(PowerupCarrier, setDirection);
    REGISTER_SCRIPT_CLASS_CALLBACK(PowerupCarrier, destroyed);
}

PowerupCarrier::PowerupCarrier()
: GameEntity(), Collisionable(sf::Vector2f(28, 20))
{
    direction = 1.0;
    speed = 40;
    textureManager.setTexture(sprite, "replicator2", 0);
    setPosition(sf::Vector2f(-20, 0));
}

PowerupCarrier::~PowerupCarrier()
{
}

void PowerupCarrier::setDirection(int dir)
{
    if (dir > 0)
    {
        setPosition(sf::Vector2f(-20, 0));
        direction = 1.0;
    }else{
        setPosition(sf::Vector2f(340, 0));
        direction = -1.0;
    }
}

void PowerupCarrier::update(float delta)
{
    if (direction > 0)
        setPosition(sf::Vector2f(getPosition().x + delta * speed, sin(getPosition().x / 30) * 30 + 50));
    else
        setPosition(sf::Vector2f(getPosition().x - delta * speed, sin(getPosition().x / 30 + M_PI) * 30 + 50));
    if ((direction > 0 && getPosition().x > 320 + 20) || (direction < 0 && getPosition().x < -20))
    {
        destroyed();
        destroy();
    }
}

bool PowerupCarrier::takeDamage(sf::Vector2f position, int damageType, int damageAmount)
{
    if (damageType >= 0)
        return false;
    destroyed();
    destroy();
    new Explosion(getPosition(), 12);
    P<Powerup> powerup = new Powerup();
    powerup->setPosition(getPosition());
    return true;
}

void PowerupCarrier::render(sf::RenderTarget& window)
{
    sf::Sprite powerupSprite;
    textureManager.setTexture(powerupSprite, "robot", 0);
    powerupSprite.setPosition(getPosition());
    powerupSprite.setScale(0.1, 0.1);
    window.draw(powerupSprite);

    sprite.setPosition(getPosition());
    window.draw(sprite);
}
