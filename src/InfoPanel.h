#pragma once
#include "Utils.h"

class InfoPanel
{
public:
    InfoPanel();
    ~InfoPanel();

    void OnInit(const sf::Font& _font, int _bombsNum);
    void OnFlagSet();
    void OnFlagUnset();

    void OnGameStart();
    void OnGameFinish();

    void Update(float _dt);
    void Render(sf::RenderWindow& _window);

private:

    void updateBombsLeftText(int _newNumber);
    
    sf::Text m_text;
    sf::Text m_finishText;
    int m_bombsLeft = 0;
};

