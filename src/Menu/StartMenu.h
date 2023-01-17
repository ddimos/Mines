#pragma once
#include "BaseMenu.h"

class StartMenu : public BaseMenu
{
public:
    StartMenu();
    ~StartMenu() = default;

private:
    void onActive() override {}
    void onDeactive() override {}
    void onDraw(sf::RenderWindow& _window) override;

    sf::Text m_text;
};

