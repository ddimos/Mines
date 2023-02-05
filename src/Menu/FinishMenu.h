#pragma once

#include "BaseMenu.h"

class FinishMenu : public BaseMenu
{
public:
    FinishMenu();

private:
    void onDraw(sf::RenderWindow& _window) override;
    void onActivate() override;

    sf::Text m_mainText;
    sf::Text m_descriptionText;
};
