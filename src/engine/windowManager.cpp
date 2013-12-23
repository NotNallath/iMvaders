#include "windowManager.h"
#include "Updatable.h"
#include "Renderable.h"
#include "Collisionable.h"
#include "PostProcessManager.h"
#include "input.h"

WindowManager::WindowManager(int virtualWidth, int virtualHeight, bool fullscreen)
: virtualSize(virtualWidth, virtualHeight)
{
    srand(time(NULL));

    // Create the window of the application
    int windowWidth = virtualWidth;
    int windowHeight = virtualHeight;
    
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    if (fullscreen)
    {
        windowWidth = desktop.width;
        windowHeight = desktop.height;
    }else{
        unsigned int scale = 2;
        while(windowWidth * scale < desktop.width && windowHeight * scale < desktop.height)
            scale += 1;
        windowWidth *= scale - 1;
        windowHeight *= scale - 1;
    }

    window.create(sf::VideoMode(windowWidth, windowHeight, 32), "iMvaders!", sf::Style::None);
    window.setVerticalSyncEnabled(false);
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(false);
    sf::View view(sf::Vector2f(virtualWidth/2,virtualHeight/2), sf::Vector2f(virtualWidth, virtualHeight));
    if (windowWidth > windowHeight * virtualWidth / virtualHeight)
    {
        float aspect = float(windowHeight) * float(virtualWidth) / float(virtualHeight) / float(windowWidth);
        float offset = 0.5 - 0.5 * aspect;
        view.setViewport(sf::FloatRect(offset, 0, aspect, 1));
    }else{
        float aspect = float(windowWidth) / float(windowHeight) * float(virtualHeight) / float(virtualWidth);
        float offset = 0.5 - 0.5 * aspect;
        view.setViewport(sf::FloatRect(0, offset, 1, aspect));
    }
    window.setView(view);
    postProcessorManager.setVirtualSize(virtualSize);
    inputHandler.setWindow(this);
}

WindowManager::~WindowManager()
{
}

void WindowManager::mainLoop()
{
    sf::Clock frameTimeClock;
    while (window.isOpen())
    {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Window closed or escape key pressed: exit
            if ((event.type == sf::Event::Closed) ||
               ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
            {
                window.close();
                break;
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            window.close();

        float delta = frameTimeClock.getElapsedTime().asSeconds();
        frameTimeClock.restart();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab))
            delta /= 5.0;
        foreach(Updatable, u, updatableList)
            u->update(delta);
        Collisionable::handleCollisions();

        // Clear the window
        window.clear(sf::Color(0, 0, 0));
        sf::RenderTarget& renderTarget = postProcessorManager.getPrimaryRenderTarget(window);
        renderTarget.clear(sf::Color(0, 0, 0));
        foreach(Renderable,r,renderableList)
            r->preRender(renderTarget);
        foreach(Renderable,r,renderableList)
            r->render(renderTarget);
        foreach(Renderable,r,renderableList)
            r->postRender(renderTarget);
        
        postProcessorManager.postProcessRendering(window);

        // Display things on screen
        window.display();
    }
}
