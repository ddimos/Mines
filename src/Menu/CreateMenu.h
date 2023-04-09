#pragma once
#include "BaseMenu.h"

class CreateMenu : public BaseMenu
{
public:
    CreateMenu();
    ~CreateMenu() = default;

private:
    void onDraw(sf::RenderWindow& _window) override;
    void onColorButtonChosen(unsigned _buttonNum, bool _isChosen);

    ChoosableButton* m_chosenColorButton = nullptr;

    sf::Text m_enterNameText;
    sf::Text m_chooseColorText;

    std::string m_enteredName = "";
    unsigned m_chosenColor = 0;
};
