#pragma once

namespace sf   
{
    class RenderWindow;
}

class Application
{
public:
    bool StartUp(sf::RenderWindow* window);
    bool ShutDown();

    void MainLoop();

private:
    sf::RenderWindow* m_window = nullptr;
};