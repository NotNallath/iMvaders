#ifndef BASICENEMYBASE_H
#define BASICENEMYBASE_H
#include "curve.h"
#include "gameEntity.h"
#include "random.h"
#include "Collisionable.h"
#include "textureManager.h"
#include "explosion.h"
#include "bullet.h"
#include "scriptInterface.h"

enum EnemyState
{
    ES_Wait,
    ES_FlyIn,
    ES_CenterField,
    ES_Diving,
    ES_Outside
};

/* Define script conversion function for the EnemyState enum. */
template<> void convert<EnemyState>::param(lua_State* L, int& idx, EnemyState& es);

class BasicEnemyBase: public GameEntity, public Collisionable
{
public:
    EnemyState state;
    Curve flyInCurve[2];
    Curve diveCurve;
    sf::Vector2f targetPosition;
    bool hasShield;
    float shieldPower;
    int flyIncurveNr, flyIncurveCount;
    static const float shieldMaxPower = 0.5;
    static const float flySpeed = 120.0;
    float enemyOffset;
    sf::Color color;

public:
    BasicEnemyBase()
    : GameEntity(), Collisionable(8.0f)
    {
        state = ES_Outside;
        flyIncurveCount = 0;
        enemyOffset = 0;

        textureManager.setTexture(sprite, "BasicEnemy", 0);
        color = sf::Color(212, 0, 0, 255);
        sprite.setColor(color);
        setPosition(sf::Vector2f(-50, -50));
        hasShield = false;
    }
    virtual ~BasicEnemyBase();
    virtual void update(float delta);
    virtual void collision(Collisionable* target);
    
    void setTargetPosition(sf::Vector2f targetPosition) { this->targetPosition = targetPosition; }

    void dive(sf::Vector2f target);
    void wait(sf::Vector2f start);
    void wait(sf::Vector2f start, sf::Vector2f flyByTarget);
    void flyIn();
    void giveShield();

    virtual void render(sf::RenderTarget& window);
    bool shieldUp();

    virtual bool takeDamage(sf::Vector2f position, int damageType, int damageAmount);
};
#endif
