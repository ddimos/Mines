#pragma once
#include "BaseMenu.h"

class CreateMenu : public BaseMenu
{
public:
    CreateMenu();
    ~CreateMenu() = default;

private:
    void onActive() override {}
    void onDeactive() override {}
    void onDraw(sf::RenderWindow& _window) override;
    
    sf::Text m_text;


    std::string m_enteredStr;
};

