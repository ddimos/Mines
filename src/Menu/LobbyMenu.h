#pragma once
#include "BaseMenu.h"

class LobbyMenu : public BaseMenu
{
public:
    LobbyMenu();
    ~LobbyMenu() = default;

private:
    void onDraw(sf::RenderWindow& _window) override;
    void onActivate() override;

    sf::Sprite m_background;
    sf::Text m_waitingText;
    sf::Text m_descriptionText;

    MenuItem* m_startButton = nullptr;
    MenuItem* m_configButton = nullptr;
};
