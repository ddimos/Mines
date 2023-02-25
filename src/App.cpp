#include "App.h"
#include "Utils/Log.h"
#include "Constants.h"
#include "Utils/Config.h"
#include "Game.h"
#include "Utils/ResourceManager.h"
#include "Network/Network.h"

#include <SFML/Graphics.hpp>

const double DT = 1.f / 60.f;

bool Application::StartUp(sf::RenderWindow* window)
{
    Config::StartUp("gameConfig.conf");
    Logger::StartUp();
    
    LOG(Logger::Type::SYSTEM, "StartUp");
    m_window = window;

    if (!m_window)
    {
        LOG_ERROR(Logger::Type::SYSTEM, "window is nullptr");
        return false;
    }
    ResourceManager::StartUp();
    Network::StartUp(Config::Get().GetConfig("port", DEFAULT_PORT));
    Game::StartUp(*window);

    Game::Get().Init();
    return true;
}

void Application::MainLoop()
{
    sf::Clock clock;
    sf::Clock throttlingClock;
    float accumulator = 0.f;

    while (m_window->isOpen())
    {
        sf::Time elapsed = clock.restart();
        sf::Time elapsedLoop = throttlingClock.restart();

        accumulator += elapsed.asSeconds();

        sf::Event event;
        while (m_window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                m_window->close();
            }
            if (event.type == sf::Event::TextEntered)
            {
                Game::Get().OnTextEntered(event.text.unicode);
            }
        }

        while (accumulator >= DT)
        {
            Network::Get().Update(DT);
            Game::Get().Update(DT);
            accumulator -= DT;
        }

        m_window->clear(BACKGROUND_COLOR);
        Game::Get().Draw(*m_window);
        m_window->display();

        elapsedLoop = throttlingClock.restart();
        sf::Time t = sf::seconds(DT-elapsedLoop.asSeconds());
        if (t.asMilliseconds() > 0)
            sf::sleep(t);
    }
}

bool Application::ShutDown()
{
    LOG(Logger::Type::SYSTEM, "ShutDown");
    m_window = nullptr;

    Game::ShutDown();
    Network::ShutDown();
    Config::ShutDown();

    return true;
}
