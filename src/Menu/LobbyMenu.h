#pragma once
#include "BaseMenu.h"

class LobbyMenu : public BaseMenu
{
public:
    LobbyMenu();
    ~LobbyMenu() = default;

private:
    void onDraw(sf::RenderWindow& _window) override;

    sf::Text m_Text;
};

