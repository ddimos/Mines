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
    void onFieldSizeButtonChosen(unsigned _buttonNum, bool _isChosen);
    void onColorButtonChosen(unsigned _buttonNum, bool _isChosen);

    ChoosableButton* m_chosenFieldSizeButton = nullptr;
    ChoosableButton* m_chosenColorButton = nullptr;

    sf::Text m_chooseFieldSizeText;
    sf::Text m_enterFieldSizeText;
    sf::Text m_enterNameText;
    sf::Text m_chooseColorText;

    sf::Sprite m_spriteX;
    sf::Sprite m_spriteMine;

    std::string m_enteredStr;
};

