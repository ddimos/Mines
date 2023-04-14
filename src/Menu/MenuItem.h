#pragma once
#include <functional>
#include <string>
#include <SFML/Graphics.hpp>
#include "Constants.h"

class MenuItem
{
public:
    static constexpr float CENTER_ALLIGNED = -1.f;     // TODO not the greatest idea
    
    virtual ~MenuItem() = default;

    void Update();
    void Draw(sf::RenderWindow& _window);

    void Activate();
    void Deactivate();

protected:
    virtual void onUpdate() {}
    virtual void onDraw(sf::RenderWindow& _window) {(void)_window;}
    virtual void onActivate() {}
    virtual void onDeactivate() {}

    bool m_isActivated = false;
};

// ---------------------------------------------------------

class Interactable
{
public:
    virtual ~Interactable() = default;

    bool isHovered() const { return m_isHovered; }
    bool isButtonPressed() const { return m_isButtonPressed; }

protected:
    virtual sf::FloatRect getBounds() const = 0;

    void onUpdate();
    virtual void onHover(bool _isHoverOn) {(void)_isHoverOn;}
    virtual void onClick(bool _isButtonPressed) {(void)_isButtonPressed;}

private:
    bool m_isHovered = false;
    bool m_isButtonPressed = false;
};

// ---------------------------------------------------------

class Button : public MenuItem, public Interactable
{
public:
    using ClickCallback = std::function<void()>;

    Button(sf::Vector2f _position,
        const sf::Texture& _texture,
        const sf::IntRect& _textureRectDefault,
        const sf::IntRect& _textureRectHover,
        ClickCallback _onClickCallback);
    
protected:
    sf::FloatRect getBounds() const override;

    void onUpdate() override;
    void onDraw(sf::RenderWindow& _window) override;
    void onHover(bool _isHoverOn) override;
    void onClick(bool _isButtonPressed) override;

    sf::Sprite m_sprite;

    sf::IntRect m_textureRectDefault = {};
    sf::IntRect m_textureRectHover = {};
    ClickCallback m_onClickCallback = {};
};

// ---------------------------------------------------------
class SetOfChoosableButtons;
class ChoosableButton : public Button
{
public:
    using ChooseCallback = std::function<void(ChoosableButton&, bool)>;

    ChoosableButton(sf::Vector2f _position,
        const sf::Texture& _texture,
        const sf::IntRect& _textureRectDefault,
        const sf::IntRect& _textureRectChoose,
        ChooseCallback _onChooseCallback);
    

private:
    friend class SetOfChoosableButtons;
    void unchoose();

    void onHover(bool _isHoverOn) override {(void)_isHoverOn;}
    void onClick(bool _isButtonPressed) override;

    void onChoose();
    
    bool m_isChosen = false;
    sf::IntRect m_textureRectChoose = {};
    ChooseCallback m_onChooseCallback = {};
};

// ---------------------------------------------------------

class InputField : public MenuItem, public Interactable
{
public:
    using ValidateEnteredTextCallback = std::function<bool(sf::Uint32, const std::string&)>;
    using FinishEnterTextCallback = std::function<void(const std::string&)>;
    
    InputField(
        sf::Vector2f _position,
        const sf::Texture& _texture,
        const sf::Font& _font,
        const std::string& _helpText,   // TODO to pass a default value
        ValidateEnteredTextCallback _onValidateEnteredTextCallback,
        FinishEnterTextCallback _onFinishEnterTextCallback,
        unsigned _maxSize = DEFAULT_MAX_INPUT_FIELD_SIZE);

    void Fill(const std::string& _text);
    
private:
    sf::FloatRect getBounds() const override;

    void onUpdate() override;
    void onDraw(sf::RenderWindow& _window) override;
    void onClick(bool _isButtonPressed) override;

    void onStartEntering();
    void onFinishEntering();

    void processChar(sf::Uint32 _char);
    
    sf::Sprite m_sprite;
    sf::Text m_inputText;
    std::string m_enteredStr;
    std::string m_helpText;

    bool m_isInInputMode = false;
    ValidateEnteredTextCallback m_onValidateEnteredTextCallback = {};
    FinishEnterTextCallback m_onFinishEnterTextCallback = {};

    const unsigned m_maxSize = DEFAULT_MAX_INPUT_FIELD_SIZE;

    bool m_isPasted = false;
};

// ---------------------------------------------------------

class SetOfChoosableButtons : public MenuItem
{
public:
    using ChooseCallback = std::function<void(unsigned, bool)>;

    SetOfChoosableButtons(
        unsigned _numberOfButtons,
        sf::Vector2f _positionOfTheFirst,
        float _distanceBetweenButtons,
        const sf::Texture& _texture,
        const sf::Vector2i& _buttonDimension,
        ChooseCallback _onChooseCallback);

private:
    void onUpdate() override;
    void onDraw(sf::RenderWindow& _window) override;
    void onActivate() override;
    void onDeactivate() override;

    void onButtonChosen(ChoosableButton& _button, bool _isChosen, unsigned _num);

    std::vector<ChoosableButton> m_buttons;
    ChoosableButton* m_chosenButton = nullptr;
    ChooseCallback m_onChooseCallback = {};
};
