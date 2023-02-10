#pragma once
#include "BaseMenu.h"

class JoinMenu : public BaseMenu
{
public:
    JoinMenu();
    ~JoinMenu() = default;

private:
    void onDraw(sf::RenderWindow& _window) override;
    void onActivate() override;
    
    void onColorButtonChosen(unsigned _buttonNum, bool _isChosen);
    
    sf::Text m_enterHostAddressText;
    sf::Text m_enterNameText;
    sf::Text m_chooseColorText;
    sf::Text m_joiningText;

    std::string m_enteredAddress = "";
    std::string m_enteredName = "";
    unsigned m_chosenColor = 0;

    bool m_isJoining = false;
};
