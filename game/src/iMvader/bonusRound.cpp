#include "scriptInterface.h"
#include "random.h"
#include "textureManager.h"
#include "soundManager.h"
#include "scoreManager.h"
#include "engine.h"
#include "player.h"
#include "explosion.h"
#include "transmission.h"
#include "gameEntity.h"
#include "Collisionable.h"
#include "main.h"

class BonusRoundRow;
class BonusRound : public GameEntity
{
    float introTimeout;
    int spawnCount;
    float spawnTimeout;
    float spawnDelay;
    int scoreCount;
    PVector<BonusRoundRow> objects;
    P<Transmission> endTransmission;
public:
    ScriptCallback finished;

    BonusRound();
    virtual ~BonusRound()
    {
        //Make sure the bonus weapons are disabled if the bonusround gets destroyed by other means then a normal end.
        playerBonusWeaponsActive = false;
    }

    virtual void update(float delta);
    
    virtual void render(sf::RenderTarget& window)
    {
        if (introTimeout > 0.0)
            window.draw(sprite);
    }
    
    void score()
    {
        scoreCount++;
        soundManager.playSound("bonus_score", random(0.95, 1.05));
    }
};

class BonusRoundObject : public GameEntity, public Collisionable
{
    P<BonusRound> owner;
    float switchDelay;
    bool scored;
public:
    int type;

    BonusRoundObject(P<BonusRound> owner, int idx)
    : Collisionable(sf::Vector2f(20, 20)), owner(owner)
    {
        setPosition(sf::Vector2f(70 + 20 * idx, -10));
        setVelocity(sf::Vector2f(0, 10.0));
        
        textureManager.setTexture(sprite, "robot");
        sprite.setScale(0.25, 0.25);
        type = irandom(0, 3);
        switchDelay = 0.0;
        scored = false;
    }
    
    virtual void update(float delta)
    {
        if (switchDelay > 0)
            switchDelay -= delta;
        if (!owner)
        {
            destroy();
            return;
        }
        if (getPosition().y > 260)
            destroy();
        if (scored)
        {
            sprite.setScale(sprite.getScale() * powf(0.1, delta));
            if (sprite.getScale().x < 0.01)
                destroy();
        }
    }
    
    virtual void render(sf::RenderTarget& window)
    {
        switch(type)
        {
        case 0: sprite.setColor(sf::Color::Red); break;
        case 1: sprite.setColor(sf::Color::Green); break;
        case 2: sprite.setColor(sf::Color::Blue); break;
        case 3: sprite.setColor(sf::Color::Yellow); break;
        }
        
        sprite.setPosition(getPosition());
        sprite.setRotation(getRotation());
        window.draw(sprite);
    }
    
    virtual bool takeDamage(sf::Vector2f position, int damageType, int damageAmount)
    {
        if (scored)
            return false;
        if (damageAmount)
        {
            if (switchDelay <= 0)
            {
                type = (type + 1) % 4;
            }
            switchDelay = 0.1;
        }
        return true;
    }
    
    void score()
    {
        scored = true;
    }
};

class BonusRoundRow : public GameEntity
{
    PVector<BonusRoundObject> items;
    P<BonusRound> owner;
public:
    BonusRoundRow(P<BonusRound> owner)
    : owner(owner)
    {
        for(int n=0; n<10; n++)
        {
            items.push_back(new BonusRoundObject(owner, n));
        }
    }
    virtual ~BonusRoundRow() {}
    
    virtual void update(float delta)
    {
        int type = -1;
        foreach(BonusRoundObject, obj, items)
        {
            if (type == -1)
                type = obj->type;
            if (obj->type != type)
                return;
        }
        if (items.size() > 0)
        {
            foreach(BonusRoundObject, obj, items)
                obj->score();
            owner->score();
        }
        destroy();
    }
};

REGISTER_SCRIPT_CLASS(BonusRound)
{
    REGISTER_SCRIPT_CLASS_CALLBACK(BonusRound, finished);
}

BonusRound::BonusRound()
: GameEntity(hudLayer)
{
    textureManager.setTexture(sprite, "bonus");
    sprite.setPosition(160, 120);
    spawnCount = 10;
    spawnTimeout = 2.0;
    spawnDelay = 2.0;
    introTimeout = 3.0;
    scoreCount = 0;
    playerBonusWeaponsActive = true;
}

void BonusRound::update(float delta)
{
    if (introTimeout > 0.0)
    {
        introTimeout -= delta;
        if (introTimeout > 2.0)
            sprite.setRotation(introTimeout * 360 * 5);
        else
            sprite.setRotation(0);
    }
    if (spawnCount > 0)
    {
        if (spawnTimeout > 0.0)
        {
            spawnTimeout -= delta;
        }else{
            spawnTimeout += spawnDelay;
            objects.push_back(new BonusRoundRow(this));
            spawnCount--;
        }
    }else{
        objects.update();
        if (objects.size() < 1 && !endTransmission)
        {
            if (scoreCount > 0)
            {
                int points = scoreCount * 120;
                endTransmission = new Transmission();
                endTransmission->setFace("Henk1");
                endTransmission->setText(("Scored " + string(points) + "|bonus points").c_str());
                endTransmission->top();
                
                P<ScoreManager>(engine->getObject("score"))->add(points);
                scoreCount = 0;
            }else{
                finished();
                destroy();
                playerBonusWeaponsActive = false;
            }
        }
    }
}
