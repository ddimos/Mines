#pragma once
#include <vector>
#include <memory>
#include "MenuItem.h"

namespace sf
{
    class RenderWindow;
}

enum class MenuType
{
    None,
    START_MENU,
    CREATE_MENU,
    JOIN_MENU,
    LOBBY_MENU,
    //GAME_MENU,
    FINISH_MENU
};

class BaseMenu
{
public:
    BaseMenu(MenuType _type);
    virtual ~BaseMenu() = default;

    void Activate();
    void Deactivate();

    void Update();
    void Draw(sf::RenderWindow& _window);
    
    MenuType GetMenuType() const { return m_type; }
    bool IsActive() const { return m_isActive; }

protected:
    virtual void onActivate() {}
    virtual void onDeactivate() {}
    virtual void onUpdate() {}
    virtual void onDraw(sf::RenderWindow& _window) {(void)_window;}

    const MenuType m_type = MenuType::None;
    bool m_isActive = false;
    std::vector<std::unique_ptr<MenuItem>> m_menuItems;

};
