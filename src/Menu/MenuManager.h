#pragma once
#include "BaseMenu.h"
#include <unordered_map>
#include <memory>

namespace sf
{
    class RenderWindow;
}

class MenuManager
{
public:
    MenuManager(sf::RenderWindow& _window);
    ~MenuManager() = default;

    void OnInit();

    void Push(MenuType _menuType);
    void Pop();

    void Update(float _dt);
    void Draw(sf::RenderWindow& _window);

private:

    sf::RenderWindow& m_window;
    BaseMenu* m_currentMenu = nullptr;
    std::unordered_map<MenuType, std::unique_ptr<BaseMenu>> m_menus;
};

