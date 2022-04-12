#include "InfoPanel.h"
#include <string>

InfoPanel::InfoPanel(/* args */)
{
    m_text.setPosition(sf::Vector2f{20.f, 20.f});
    m_text.setFillColor(sf::Color::Magenta);

    m_bombsNumText.setPosition(sf::Vector2f{20.f, 200.f});
    m_bombsNumText.setFillColor(sf::Color::Magenta);

    m_enteredText.setPosition(sf::Vector2f{20.f, 250.f});
    m_enteredText.setFillColor(sf::Color::Magenta);
}

InfoPanel::~InfoPanel()
{
}

void InfoPanel::updateBombsLeftText(int _newNumber)
{
    m_bombsLeft = _newNumber;
    m_bombsNumText.setString("Bombs Left\n" + std::to_string(_newNumber));
}

void InfoPanel::OnTextEntered(const std::string& _text)
{
    m_enteredText.setString(_text);
}

void InfoPanel::OnInit(const sf::Font& _font)
{
    m_text.setFont(_font);
    m_bombsNumText.setFont(_font);
    m_enteredText.setFont(_font);
}

void InfoPanel::OnGameInit()
{
    m_text.setString("Welcome\n\nStart - B\n");
}

void InfoPanel::OnFlagSet()
{
    updateBombsLeftText(m_bombsLeft - 1);
}

void InfoPanel::OnFlagUnset()
{
    updateBombsLeftText(m_bombsLeft + 1);
}

void InfoPanel::OnGameStart(int _bombsNum)
{
    updateBombsLeftText(_bombsNum);
    m_text.setString("PLAY!!\n\nMovement - Arrows\nUncover - Space\nFlag/Unflag - X");
    m_enteredText.setString("");
}

void InfoPanel::OnGameFinish()
{
    m_text.setString("To restart press R!!");
    m_bombsNumText.setString("");
}

void InfoPanel::Update(float _dt)
{
(void)_dt;
}

void InfoPanel::Render(sf::RenderWindow& _window)
{
    _window.draw(m_text);
    _window.draw(m_bombsNumText);
    _window.draw(m_enteredText);
}
    