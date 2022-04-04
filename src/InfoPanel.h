#pragma once
#include "Utils.h"

class InfoPanel
{
public:
    InfoPanel();
    ~InfoPanel();

    void OnInit(const sf::Font& _font);
    void OnFlagSet();
    void OnFlagUnset();

    void OnGameInit();
    void OnGameStart(int _bombsNum);
    void OnGameFinish();

    void Update(float _dt);
    void Render(sf::RenderWindow& _window);

private:

    void updateBombsLeftText(int _newNumber);
    
    sf::Text m_text;
    sf::Text m_bombsNumText;

    int m_bombsLeft = 0;
};

