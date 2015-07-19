#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "gameEntity.h"
#include "EnemyGroup.h"

class MainMenu : public GameEntity
{
    constexpr static float introTextDelay = 30.0f;
    constexpr static float intoTextSpeed = 10.0f;
    float blink;
    float introTextPosition;
public:
    sf::Sprite logoSprite;
    P<EnemyGroup> enemyGroup;

    MainMenu();
    virtual ~MainMenu();

    virtual void update(float delta);

    virtual void render(sf::RenderTarget& window);
};

#endif//MAIN_MENU_H
