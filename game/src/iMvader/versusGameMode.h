#ifndef VERSUS_GAME_MODE_H
#define VERSUS_GAME_MODE_H

#include "gameState.h"

class VersusGameState : public GameEntity
{
private:
    P<PlayerCraft> player[MAX_PLAYERS];
    PlayerInfo playerInfo[MAX_PLAYERS];
    float printerSpawnDelay;
    const static float printerSpawnTime = 0.8;
    float victoryDelay;
public:
    VersusGameState();
    virtual ~VersusGameState() {}

    virtual void update(float delta);

    virtual void postRender(sf::RenderTarget& window);
};

#endif//VERSUS_GAME_MODE_H
