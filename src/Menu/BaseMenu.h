#pragma once
#include <vector>
#include <functional>
#include <SFML/Graphics.hpp>

namespace sf
{
    class RenderWindow;
}

enum class MenuType
{
    None,
    START_MENU,
    CREATE_MENU,
    JOIN_MNEU,
    LOBBY_MENU,
    //GAME_MENU,
    FINISH_MENU
};


struct Button
{
    using HoverCallback = std::function<void(bool)>;
    using ClickCallback = std::function<void(bool)>;

    Button(sf::FloatRect _button, HoverCallback _onHoverCallback, ClickCallback _onClickCallback);
    
    sf::FloatRect GetGlobalBounds() const { return m_button; }
    bool IsHovered() const { return m_isHovered; }
    bool IsClicked() const { return m_isClicked; }

    void OnHover(bool _isHoverOn);
    void OnClick(bool _isClicked);

private:
    sf::FloatRect m_button;
    bool m_isHovered = false;
    bool m_isClicked = false;
    HoverCallback m_onHoverCallback = {};
    ClickCallback m_onClickCallback = {};
};


class BaseMenu
{
public:
    BaseMenu(MenuType _type);
    virtual ~BaseMenu() = default;

    void Activate();
    void Deactivate();
    void Draw(sf::RenderWindow& _window);
    
    MenuType GetMenuType() const { return m_type; }
    bool IsActive() const { return m_isActive; }
    std::vector<Button>& GetButtons() { return m_buttons; }

protected:
    virtual void onActive() {}
    virtual void onDeactive() {}
    virtual void onDraw(sf::RenderWindow& _window) {(void)_window;}

    float calculateCenterX(float _width);

    const MenuType m_type = MenuType::None;
    bool m_isActive = false;
    std::vector<Button> m_buttons;
};
