#include "engine.h"
#include "random.h"
#include "Updatable.h"
#include "Collisionable.h"

Engine* engine;

Engine::Engine()
{
    engine = this;
    initRandom();
    windowManager = NULL;
    gameSpeed = 1.0;
}
Engine::~Engine()
{
}

void Engine::registerObject(std::string name, P<PObject> obj)
{
    objectMap[name] = obj;
}

P<PObject> Engine::getObject(std::string name)
{
    if (!objectMap[name])
        return NULL;
    return objectMap[name];
}

void Engine::runMainLoop()
{
    windowManager = dynamic_cast<WindowManager*>(*getObject("windowManager"));
    sf::Clock frameTimeClock;
    while (windowManager->window.isOpen())
    {
        // Handle events
        sf::Event event;
        while (windowManager->window.pollEvent(event))
        {
            // Window closed or escape key pressed: exit
            if ((event.type == sf::Event::Closed) ||
               ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
            {
                windowManager->window.close();
                break;
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            windowManager->window.close();

        float delta = frameTimeClock.getElapsedTime().asSeconds();
        frameTimeClock.restart();
        if (delta > 0.5)
            delta = 0.5;
        if (delta < 0.001)
            delta = 0.001;
        delta *= gameSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab))
            delta /= 5.0;
        
        foreach(Updatable, u, updatableList)
            u->update(delta);
        elapsedTime += delta;
        Collisionable::handleCollisions();

        // Clear the window
        windowManager->render();
    }
}

void Engine::setGameSpeed(float speed)
{
    gameSpeed = speed;
}

float Engine::getElapsedTime()
{
    return elapsedTime;
}
