#include "App.h"
#include "Log.h"
#include "Game.h"

#include <SFML/Graphics.hpp>

const double DT = 1.f / 60.f;

bool Application::StartUp(sf::RenderWindow* window)
{
    LOG("StartUp");
    m_window = window;

    if (!m_window)
    {
        LOG_ERROR("window is nullptr");
        return false;
    }
    Game::StartUp(window);

    return true;
}

void Application::MainLoop()
{
    sf::Clock clock;
    float accumulator = 0.f;
    
    while (m_window->isOpen())
    {
        // static std::string text;
        sf::Event event;
        while (m_window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                m_window->close();
            }
            // if (event.type == sf::Event::TextEntered)
            // {
            //     text += event.text.unicode;
            //     LOG(text.c_str());

            // }
        }

        sf::Time elapsed = clock.restart();
        accumulator += elapsed.asSeconds();

        while (accumulator >= DT)
        {
            Game::Get().Update(DT);
            accumulator -= DT;
        }

        m_window->clear();
        Game::Get().Draw(m_window);

        m_window->display();
    }

}

bool Application::ShutDown()
{
    LOG("ShutDown");
    m_window = nullptr;

    Game::ShutDown();
    return true;
}
