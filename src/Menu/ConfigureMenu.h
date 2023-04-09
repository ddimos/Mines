#pragma once
#include "BaseMenu.h"

class ConfigureMenu : public BaseMenu
{
public:
    ConfigureMenu();
    ~ConfigureMenu() = default;

private:
    void onDraw(sf::RenderWindow& _window) override;
    void onFieldSizeButtonChosen(unsigned _buttonNum, bool _isChosen);
    void onGameModeButtonChosen(unsigned _buttonNum, bool _isChosen);

    ChoosableButton* m_chosenFieldSizeButton = nullptr;
    ChoosableButton* m_chosenGameModeButton = nullptr;

    sf::Text m_chooseFieldSizeText;
    sf::Text m_enterFieldSizeText;
    sf::Text m_chooseGameModeText;

    sf::Sprite m_spriteX;
    sf::Sprite m_spriteMine;

    unsigned m_enteredHeight = 0;
    unsigned m_enteredWidth = 0;
    unsigned m_enteredBombsCount = 0;
    unsigned m_enteredGameMode = 0;

    InputField* m_widthInputField = nullptr;
    InputField* m_heightInputField = nullptr;
    InputField* m_bombsCountInputField = nullptr;
};
