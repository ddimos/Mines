#pragma once
#include "BaseMenu.h"

class CreateMenu : public BaseMenu
{
public:
    CreateMenu();
    ~CreateMenu() = default;

private:
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

    std::string m_enteredName = "";
    unsigned m_enteredHeight = 0;
    unsigned m_enteredWidth = 0;
    unsigned m_enteredBombsCount = 0;
    unsigned m_chosenColor = 0;

    InputField* m_widthInputField = nullptr;
    InputField* m_heightInputField = nullptr;
    InputField* m_bombsCountInputField = nullptr;
};

