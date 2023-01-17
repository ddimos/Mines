#pragma once
#include <functional>
#include <string>
#include <SFML/Graphics.hpp>


class MenuItem
{
public:
    static constexpr float CENTER_ALLIGNED = -1.f;     // TODO not the greatest idea
    
    virtual ~MenuItem() = default;

    void Update();
    void Draw(sf::RenderWindow& _window);

protected:
    virtual sf::FloatRect getBounds() const = 0;
    
    virtual void onUpdate() {}
    virtual void onDraw(sf::RenderWindow& _window) {(void)_window;}
    virtual void onHover(bool _isHoverOn) {(void)_isHoverOn;}
    virtual void onClick(bool _isButtonPressed) {(void)_isButtonPressed;}

    bool isHovered() const { return m_isHovered; }
    bool isButtonPressed() const { return m_isButtonPressed; }

private:
    bool m_isHovered = false;
    bool m_isButtonPressed = false;
};

// ---------------------------------------------------------

class Button : public MenuItem
{
public:
    using ClickCallback = std::function<void()>;

    Button(sf::Vector2f _position,
        const sf::Texture& _texture,
        const sf::IntRect& _textureRectDefault,
        const sf::IntRect& _textureRectHover,
        ClickCallback _onClickCallback);
    
private:
    sf::FloatRect getBounds() const override;

    void onDraw(sf::RenderWindow& _window) override;
    void onHover(bool _isHoverOn) override;
    void onClick(bool _isButtonPressed) override;

    sf::Sprite m_sprite;

    sf::IntRect m_textureRectDefault = {};
    sf::IntRect m_textureRectHover = {};
    ClickCallback m_onClickCallback = {};
};

// ---------------------------------------------------------

class InputField : public MenuItem
{
public:
    using ValidateEnteredTextCallback = std::function<bool(sf::Uint32)>;
    using FinishEnterTextCallback = std::function<void(const std::string&)>;
    
    InputField(
        sf::Vector2f _position,
        const sf::Texture& _texture,
        const sf::Font& _font,
        const std::string& _helpText,
        ValidateEnteredTextCallback _onValidateEnteredTextCallback,
        FinishEnterTextCallback _onFinishEnterTextCallback);

private:
    sf::FloatRect getBounds() const override;

    void onUpdate() override;
    void onDraw(sf::RenderWindow& _window) override;
    void onClick(bool _isButtonPressed) override;

    sf::Sprite m_sprite;
    sf::Text m_inputText;
    std::string m_enteredStr;

    bool m_isInInputMode = false;
    ValidateEnteredTextCallback m_onValidateEnteredTextCallback = {};
    FinishEnterTextCallback m_onFinishEnterTextCallback = {};
};
