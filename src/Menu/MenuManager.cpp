#include "Menu/MenuManager.h"
#include "Menu/StartMenu.h"
#include "Log.h"
#include <SFML/Window.hpp>

MenuManager::MenuManager(sf::RenderWindow& _window)
    : m_window(_window)
{
}

MenuManager::~MenuManager()
{
}

void MenuManager::OnInit()
{
    m_menus.insert_or_assign(MenuType::CREATE_MENU, std::make_unique<StartMenu>());

}

void MenuManager::Push(MenuType _menuType)
{
    if (m_currentMenu)
    {
        LOG_ERROR("Multiple menus is not supported in the stack");

        m_currentMenu->Deactivate();
    }
    auto it = m_menus.find(_menuType);
    if (it == m_menus.end())
    {
        LOG_ERROR("Menu wasn't found"); // TODO add a name
        return;
    }
    
    m_currentMenu = it->second.get();
    m_currentMenu->Activate();
}

void MenuManager::Pop()
{
    if (m_currentMenu)
        m_currentMenu->Deactivate();

    m_currentMenu = nullptr;
}

void MenuManager::Update(float _dt)
{
    (void)_dt;

    if (!m_currentMenu || !m_currentMenu->IsActive())
        return;
    if (!m_window.hasFocus())
        return;

    auto& buttons = m_currentMenu->GetButtons();
    const sf::Vector2f mousePosition = sf::Vector2f(sf::Mouse::getPosition(m_window));

    for (auto& button : buttons)
    {
        if (button.GetGlobalBounds().contains(mousePosition))
        {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                if (!button.IsClicked())
                    button.OnClick(true);
            }
            else
            {
                if (button.IsClicked())
                    button.OnClick(false);
                if (!button.IsHovered())
                    button.OnHover(true);
            }
        }
        else
        {
            if (button.IsHovered())
                button.OnHover(false);
        }

    }
    
}

void MenuManager::Draw(sf::RenderWindow& _window)
{
    if (m_currentMenu)
        m_currentMenu->Draw(_window);
}
