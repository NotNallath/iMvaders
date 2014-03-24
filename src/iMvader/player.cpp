#include <math.h>

#include "postProcessManager.h"
#include "player.h"
#include "textureManager.h"
#include "vectorUtils.h"
#include "bullet.h"
#include "explosion.h"
#include "nuke.h"

PlayerCraft::PlayerCraft(PlayerController* controller, PlayerInfo* info, int type)
: GameEntity(), Collisionable(10.0), controller(controller), info(info), type(type)
{
    invulnerability = 1.0;
    fireCooldown = 0.4;
    nukeCooldown = 2.0;
    chargeShot = 0;
    health = 2;
    if (type == 0)
        textureManager.setTexture(sprite, "player1");
    else
        textureManager.setTexture(sprite, "player2");
    setPosition(sf::Vector2f(160, 220));
}

PlayerCraft::~PlayerCraft()
{
}

void PlayerCraft::update(float delta)
{
    if (fireCooldown > 0)
        fireCooldown -= delta;
    if (nukeCooldown > 0)
        nukeCooldown -= delta;
    if (invulnerability > 0)
        invulnerability -= delta;

    velocity = velocity * 0.85f;//TODO: Proper dampening with use of delta.
    if (controller->left())
        velocity = sf::Vector2f(-100.0, velocity.y);
    if (controller->right())
        velocity = sf::Vector2f( 100.0, velocity.y);
    if (controller->up())
        velocity = sf::Vector2f(velocity.x, -100);
    if (controller->down())
        velocity = sf::Vector2f(velocity.x,  100);

    setPosition(getPosition() + velocity * delta);

    if (getPosition().x < 20)
        setPosition(sf::Vector2f(20, getPosition().y));
    if (getPosition().x > 300)
        setPosition(sf::Vector2f(300, getPosition().y));
    if (getPosition().y < 10)
        setPosition(sf::Vector2f(getPosition().x, 10));
    if (getPosition().y > 230)
        setPosition(sf::Vector2f(getPosition().x, 230));

    if (controller->button(chargeShotButton))
    {
        chargeShot += delta;
        if (chargeShot > maxChargeShot) chargeShot = maxChargeShot;
    }else if (chargeShot > 0)
    {
        int shots = 5 * (chargeShot - minChargeShot) / (maxChargeShot - minChargeShot);
        chargeShot = 0.0;
        for(int n=0; n<=shots; n++)
        {
            new Bullet(getPosition(), -1 - type, (float(n) - float(shots) / 2.0) / float(shots) * 15.0);
        }
    }
    
    if (controller->button(fireButton) && fireCooldown <= 0 && invulnerability <= 0 && !controller->button(chargeShotButton))
    {
        if (type == 0)
        {
            new Bullet(getPosition(), -1, 0);
            fireCooldown = 0.4;
        }
        if (type == 1)
        {
            new Bullet(getPosition() + sf::Vector2f(7, 0), -2, 0);
            new Bullet(getPosition() + sf::Vector2f(-4, 0), -2, 0);
            fireCooldown = 0.8;
        }
    }
    if (!controller->button(fireButton))
    {
        if (type == 0 && fireCooldown > 0.1)
            fireCooldown = 0.1;
        if (type == 1 && fireCooldown > 0.25)
            fireCooldown = 0.25;
    }
    if (nukeCooldown <= 0 && controller->button(nukeButton) && info->nukes > 0 && !controller->button(chargeShotButton))
    {
        nukeCooldown = 2.0;
        info->nukes -= 1;
        new Nuke(getPosition(), sf::Vector2f(0.0, -150.0), 10.0, type);
    }
}

void PlayerCraft::render(sf::RenderTarget& window)
{
    if (fmod(invulnerability, 4.0/60.0) > 2.0/60.0)
        return;
    sprite.setPosition(getPosition());
    sprite.setRotation(velocity.x / 10.0);
    window.draw(sprite);
    
    if (chargeShot > minChargeShot)
    {
        float r = 0.5 + 3 * (chargeShot - minChargeShot) / (maxChargeShot - minChargeShot);
        sf::CircleShape circle(r, random(3, 5));
        circle.setOrigin(r, r);
        if (type == 0)
            circle.setFillColor(sf::Color(24, 161, 212, 200));
        else
            circle.setFillColor(sf::Color(231, 24, 118, 200));
        circle.setRotation(random(0, 360));
        circle.setPosition(getPosition() + sf::vector2FromAngle(velocity.x / 10.0f) * 10.0f);
        window.draw(circle);
    }
}

bool PlayerCraft::takeDamage(sf::Vector2f position, int damageType, int damageAmount)
{
    if (damageType < 0 || invulnerability > 0)
        return false;
    health -= damageAmount;
    invulnerability = 1.0;
    if(health <= 0)
    {
        postProcessorManager.triggerPostProcess("pixel", 1.0);
        destroy();
        for(unsigned int n=0; n<4; n++)
        {
            new Explosion(sprite.getPosition() + sf::Vector2f(random(-10, 10), random(-10, 10)), 10);
        }
        //new Explosion(getPosition(), 12);
    }
    return true;
}
