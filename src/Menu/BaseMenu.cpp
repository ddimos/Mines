#include "BaseMenu.h"
#include "Game.h"


BaseMenu::BaseMenu(MenuType _type)
    : m_type(_type)
{
}

void BaseMenu::Activate()
{
    m_isActive = true;
    for (auto& item : m_menuItems)
        item->Activate();
    onActivate();
}

void BaseMenu::Deactivate() 
{
    m_isActive = false;
    for (auto& item : m_menuItems)
        item->Deactivate();
    onDeactivate();
}

void BaseMenu::Update()
{
    for (auto& item : m_menuItems)
        item->Update();
    onUpdate();
}

void BaseMenu::Draw(sf::RenderWindow& _window)
{
    onDraw(_window);
    for (auto& item : m_menuItems)
        item->Draw(_window);
}
