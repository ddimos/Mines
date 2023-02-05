#include "Menu/MenuManager.h"
#include "Menu/StartMenu.h"
#include "Menu/CreateMenu.h"
#include "Menu/JoinMenu.h"
#include "Menu/LobbyMenu.h"
#include "Menu/FinishMenu.h"
#include "Log.h"

MenuManager::MenuManager(sf::RenderWindow& _window)
    : m_window(_window)
{
}

MenuManager::~MenuManager()
{
}

void MenuManager::OnInit()
{
    m_menus.insert_or_assign(MenuType::START_MENU, std::make_unique<StartMenu>());
    m_menus.insert_or_assign(MenuType::CREATE_MENU, std::make_unique<CreateMenu>());
    m_menus.insert_or_assign(MenuType::JOIN_MENU, std::make_unique<JoinMenu>());
    m_menus.insert_or_assign(MenuType::LOBBY_MENU, std::make_unique<LobbyMenu>());
    m_menus.insert_or_assign(MenuType::FINISH_MENU, std::make_unique<FinishMenu>());
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

    m_currentMenu->Update();
}

void MenuManager::Draw(sf::RenderWindow& _window)
{
    if (m_currentMenu)
        m_currentMenu->Draw(_window);
}
