#ifndef BRE_H
#define BRE_H

#include "gameEntity.h"
#include "BasicEnemy.h"

enum BreState
{
    BS_FlyIn,
    BS_MoveLeftRight,
    BS_LaserCharge,
    BS_LaserFire,
    BS_MouthOpen,
    BS_MouthClose,
};

class BreLaser;
class BreEnemy: public GameEntity, public Collisionable
{
public:
    sf::Sprite mouth;
    BreState state;
    int moveDir;
    int health;
    float shotDelay;
    float moveSpeed;
    float mouthPos;
    float invulnerability;
    int enemySpawnCount;
    int difficulty;
    bool moneyshieldDeployed;
    PVector<BasicEnemyBase> enemyList;
    P<BreLaser> laser[2];
    ScriptCallback destroyed;

    static const int maxHealth = 100;
    static const int normalShotDelay = 1.2;
    static const int laserChargeTime = 2.0;

    BreEnemy();
    virtual ~BreEnemy();

    void setDifficulty(int difficulty);
    
    virtual void update(float delta);
    virtual void render(sf::RenderTarget& window);
    virtual void postRender(sf::RenderTarget& window);
    virtual bool takeDamage(sf::Vector2f position, int damageType, int damageAmount);
};

class BreLaser: public GameEntity, public Collisionable
{
    P<BreEnemy> owner;
public:
    BreLaser(P<BreEnemy> owner);

    virtual void update(float delta);
    virtual void render(sf::RenderTarget& window);

    virtual void collision(Collisionable* other);
};

class MoneyShield: public GameEntity, public Collisionable
{
    float angle;
    float distance;
    float endDistance;
    bool counterClockwise;
    
    P<BreEnemy> owner;
public:
    MoneyShield(P<BreEnemy> owner, float startAngle, float endDistance, bool counterClockwise);
    
    virtual void update(float delta);
    virtual void render(sf::RenderTarget& window);
    
    virtual bool takeDamage(sf::Vector2f position, int damageType, int damageAmount);
};

#endif // BRE_H
