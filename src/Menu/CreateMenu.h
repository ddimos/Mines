#pragma once
#include "BaseMenu.h"

class CreateMenu : public BaseMenu
{
public:
    enum class WorldSize
    {
        OPTION_0,
        OPTION_1,
        OPTION_2
    };
    enum class Color
    {
        OPTION_0,
        OPTION_1,
        OPTION_2,
        OPTION_3,
        OPTION_4,
        OPTION_5,
        OPTION_6,
        OPTION_7,
    };
    CreateMenu();
    ~CreateMenu() = default;

private:
    void onActive() override {}
    void onDeactive() override {}
    void onDraw(sf::RenderWindow& _window) override;
    void onFieldSizeButtonChosen(ChoosableButton& _button, bool _isChosen, WorldSize _worldSize);
    void onColorButtonChosen(ChoosableButton& _button, bool _isChosen, Color _color);

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

