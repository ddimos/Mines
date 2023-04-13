#pragma once
#include <vector>
#include "BaseMenu.h"

class FinishMenu : public BaseMenu
{
public:
    FinishMenu();

private:
    void onDraw(sf::RenderWindow& _window) override;
    void onActivate() override;

    struct StatsText
    {
        sf::Text name;
        sf::Text died;
        sf::Text mines;
    };

    sf::Sprite m_background;
    sf::Text m_mainText;
    sf::Text m_descriptionText;
    std::vector<StatsText> m_stats;
};
