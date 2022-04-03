#include "InfoPanel.h"
#include <string>

InfoPanel::InfoPanel(/* args */)
{
    m_text.setPosition(sf::Vector2f{20.f, 20.f});
    m_text.setFillColor(sf::Color::Magenta);

    m_finishText.setPosition(sf::Vector2f{20.f, 200.f});
    m_finishText.setFillColor(sf::Color::Magenta);
}

InfoPanel::~InfoPanel()
{
}

void InfoPanel::updateBombsLeftText(int _newNumber)
{
    m_bombsLeft = _newNumber;
    m_text.setString(std::to_string(_newNumber));
}

void InfoPanel::OnInit(const sf::Font& _font, int _bombsNum)
{
    m_text.setFont(_font);
    m_finishText.setFont(_font);
    updateBombsLeftText(_bombsNum);
}

void InfoPanel::OnFlagSet()
{
    updateBombsLeftText(m_bombsLeft - 1);
}

void InfoPanel::OnFlagUnset()
{
    updateBombsLeftText(m_bombsLeft + 1);
}

void InfoPanel::OnGameStart()
{
    m_finishText.setString("PLAY!!");
}

void InfoPanel::OnGameFinish()
{
    m_finishText.setString("To restart press R!!");
}

void InfoPanel::Update(float _dt)
{
(void)_dt;
}

void InfoPanel::Render(sf::RenderWindow& _window)
{
    _window.draw(m_text);
    _window.draw(m_finishText);
}
    