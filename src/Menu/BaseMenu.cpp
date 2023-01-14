#include "BaseMenu.h"
#include "Game.h"

Button::Button(sf::FloatRect _button, HoverCallback _onHoverCallback, ClickCallback _onClickCallback)
    : m_button(_button)
    , m_onHoverCallback(_onHoverCallback)
    , m_onClickCallback(_onClickCallback)
{
}

void Button::OnHover(bool _isHovered)
{
    m_isHovered = _isHovered;
    m_onHoverCallback(_isHovered);
}

void Button::OnClick(bool _isClicked) 
{
    m_isClicked = _isClicked;
    m_onClickCallback(_isClicked);
}


BaseMenu::BaseMenu(MenuType _type)
    : m_type(_type)
{
}

void BaseMenu::Activate()
{
    m_isActive = true;
    onActive();
}

void BaseMenu::Deactivate() 
{
    m_isActive = false;
    onDeactive();
}
    
void BaseMenu::Draw(sf::RenderWindow& _window)
{
    onDraw(_window);
}

float BaseMenu::calculateCenterX(float _width)
{
    return Game::Get().GetWindow().getSize().x / 2.f - _width / 2.f;
}