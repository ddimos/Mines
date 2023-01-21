#pragma once
#include "BaseMenu.h"

class JoinMenu : public BaseMenu
{
public:
    JoinMenu();
    ~JoinMenu() = default;

private:
    void onDraw(sf::RenderWindow& _window) override;

    void onColorButtonChosen(unsigned _buttonNum, bool _isChosen);
    
    sf::Text m_enterHostAddressText;
    sf::Text m_enterNameText;
    sf::Text m_chooseColorText;

    std::string m_enteredStr;
};
