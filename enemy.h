
enum EnemyState
{
    ES_Wait,
    ES_FlyIn,
    ES_CenterField,
    ES_Diving,
    ES_Outside
};

float enemyOffset;
float enemyDirection;

class Enemy: public GameEntity
{
public:
    EnemyState state;
    Curve flyInCurve[2];
    Curve diveCurve;
    sf::Vector2f targetPosition;
    int shotDelay;
    bool hasShield;
    int shieldPower;
    int flyIncurveNr, flyIncurveCount;
    static const int shieldMaxPower = 30;
    static const int shotAngle = 120;

public:    
    Enemy(sf::Vector2f targetPosition)
    : GameEntity(8.0f), targetPosition(targetPosition)
    {
        state = ES_Outside;
        flyIncurveCount = 0;
        
        sprite.setTexture(invaderTexture, true);
        sprite.setOrigin(invaderTexture.getSize().x/2, invaderTexture.getSize().y/2);
        sprite.setPosition(sf::Vector2f(-50, -50));
        shotDelay = random(50, 500);
        hasShield = false;
    }
    virtual ~Enemy()
    {
    }
    
    virtual void update()
    {
        if (shotDelay)
        {
            shotDelay--;
        }else{
            shotDelay = random(200, 400);
            float a = sprite.getRotation();
            if (a > 180 - shotAngle/2 && a < 180 + shotAngle/2)
                new Bullet(sprite.getPosition(), 0, a);
        }
        if (hasShield && shieldPower < shieldMaxPower)
        {
            shieldPower++;
            if (shieldPower == shieldMaxPower)
                giveShield();
        }
        
        if (flyIncurveCount > 0)
            flyInCurve[flyIncurveCount-1].p1 = targetPosition + sf::Vector2f(enemyOffset, 0);
        diveCurve.p0 = targetPosition + sf::Vector2f(enemyOffset, 0);
        switch(state)
        {
        case ES_Wait:
            break;
        case ES_FlyIn:
            if (flyInCurve[flyIncurveNr].delta < 1.0)
            {
                flyInCurve[flyIncurveNr].moveDistance(2.0);
                sprite.setRotation(flyInCurve[flyIncurveNr].angle());
            }
            else
            {
                if (flyIncurveNr < flyIncurveCount - 1)
                {
                    flyIncurveNr++;
                    flyInCurve[flyIncurveNr].delta = 0.0;
                }else{
                    sprite.setRotation(180);
                    state = ES_CenterField;
                }
            }
            sprite.setPosition(flyInCurve[flyIncurveNr].getPosition());
            break;
        case ES_CenterField:
            sprite.setPosition(targetPosition + sf::Vector2f(enemyOffset, 0));
            break;
        case ES_Diving:
            if (diveCurve.delta < 1.0)
            {
                diveCurve.moveDistance(2.0);
                sprite.setRotation(diveCurve.angle());
            }
            else
            {
                state = ES_Outside;
            }
            sprite.setPosition(diveCurve.getPosition());
            break;
        case ES_Outside:
            break;
        }
    }
    
    void dive(sf::Vector2f target)
    {
        diveCurve.p0 = sprite.getPosition();
        diveCurve.cp0 = diveCurve.p0 + sf::vector2FromAngle(sprite.getRotation()) * 30.0f;
        diveCurve.p1 = target;
        diveCurve.cp1 = sf::Vector2f(target.x, 180);
        diveCurve.delta = 0.0;
        state = ES_Diving;
    }
    
    void wait(sf::Vector2f start)
    {
        state = ES_Wait;
        flyIncurveNr = 0;
        flyInCurve[0].delta = 0.0;
        flyInCurve[0].p0 = start;
        flyInCurve[0].cp0 = flyInCurve[0].p0 + sf::Vector2f(0, 30);
        flyInCurve[0].p1 = targetPosition;
        flyInCurve[0].cp1 = flyInCurve[0].p1 - sf::Vector2f(0, 30);
        flyIncurveCount = 1;
        sprite.setPosition(flyInCurve[0].p0);
    }

    void wait(sf::Vector2f start, sf::Vector2f flyByTarget)
    {
        state = ES_Wait;
        
        sf::Vector2f normal = normalize(flyByTarget - start);
        sf::Vector2f dir(-normal.y, normal.x);
        if (start.x > flyByTarget.x)
            dir = -dir;
        
        flyIncurveNr = 0;
        flyInCurve[0].delta = 0.0;
        flyInCurve[0].p0 = start;
        flyInCurve[0].cp0 = flyInCurve[0].p0 + sf::Vector2f(0, 30);
        flyInCurve[0].p1 = flyByTarget;
        flyInCurve[0].cp1 = flyByTarget + dir * 50.f;

        flyInCurve[1].p0 = flyByTarget;
        flyInCurve[1].cp0 = flyByTarget - dir * 50.f;
        flyInCurve[1].p1 = targetPosition;
        flyInCurve[1].cp1 = flyInCurve[1].p1 - sf::Vector2f(0, 50);
        flyIncurveCount = 2;
        sprite.setPosition(flyInCurve[0].p0);
    }

    void flyIn()
    {
        state = ES_FlyIn;
    }
    
    void giveShield()
    {
        collisionRadius = 12.0f;
        hasShield = true;
        shieldPower = shieldMaxPower;
        sprite.setTexture(invaderShieldedTexture, true);
        sprite.setOrigin(invaderShieldedTexture.getSize().x/2, invaderShieldedTexture.getSize().y/2);
    }
    
    virtual void render(sf::RenderTarget& window)
    {
        window.draw(sprite);

#ifdef DEBUG
        for(int n=0; n<flyIncurveCount; n++)
            flyInCurve[n].draw(window);
        diveCurve.draw(window);
#endif
    }
    
    bool shieldUp()
    {
        return hasShield && shieldPower == shieldMaxPower;
    }
    
    virtual bool takeDamage(sf::Vector2f position, int damageType, int damageAmount)
    {
        if (damageType >= 0)
            return false;
        if (shieldUp())
        {
            shieldPower = 0;
            collisionRadius = 8.0f;
            sprite.setTexture(invaderTexture, true);
            sprite.setOrigin(invaderTexture.getSize().x/2, invaderTexture.getSize().y/2);
        }
        else
        {
            new Explosion(sprite.getPosition(), 8);
            destroy();
        }
        return true;
    }
};

class EnemyGroup : public GameEntity
{
private:
    PVector<Enemy> enemyList;
public:
    EnemyGroup()
    {
    }
    
    Enemy* add(sf::Vector2f targetPoint)
    {
        Enemy* e = new Enemy(targetPoint);
        enemyList.push_back(e);
        return e;
    }
    
    virtual ~EnemyGroup() {}
    
    virtual void update()
    {
        if (enemyList.size() < 1)
            destroy();
    
        P<Enemy> prev;
        foreach(Enemy, e, enemyList)
        {
            if (e->state == ES_Wait)
            {
                if (!prev || e->sprite.getPosition() - prev->sprite.getPosition() > 20.0f)
                    e->flyIn();
                break;
            }
            if (e->state == ES_CenterField && prev && prev->state == ES_Diving)
            {
                if (e->sprite.getPosition() - prev->sprite.getPosition() > 30.0f)
                    e->dive(prev->diveCurve.p1);
                break;
            }
            prev = e;
        }
    }
    
    bool isAll(EnemyState state)
    {
        foreach(Enemy, e, enemyList)
            if (e->state != state)
                return false;
        return true;
    }
    
    void dive(sf::Vector2f target)
    {
        foreach(Enemy, e, enemyList)
        {
            e->dive(target);
            break;
        }
    }
    
    void flyIn(sf::Vector2f start)
    {
        foreach(Enemy, e, enemyList)
            e->wait(start);
        if (enemyList.size() > 0)
            enemyList[0]->flyIn();
    }

    void flyIn(sf::Vector2f start, sf::Vector2f flyByPoint)
    {
        foreach(Enemy, e, enemyList)
            e->wait(start, flyByPoint);
        if (enemyList.size() > 0)
            enemyList[0]->flyIn();
    }
};
