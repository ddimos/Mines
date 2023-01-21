#include "MenuItem.h"
#include "Game.h"
#include <SFML/Window.hpp>

namespace
{
    constexpr int INPUT_FIELD_MARGIN = 5;
}


void MenuItem::Update()
{
    onUpdate();
}

void MenuItem::Draw(sf::RenderWindow& _window)
{
    onDraw(_window);
}

// ---------------------------------------------------------

void Interactable::onUpdate()
{
    const auto& window = Game::Get().GetWindow();
    if (!window.hasFocus())    
        return;

    const sf::Vector2f mousePosition = sf::Vector2f(sf::Mouse::getPosition(window));
    if (getBounds().contains(mousePosition))
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            if (!m_isButtonPressed)
            {
                m_isButtonPressed = true;
                onClick(m_isButtonPressed);
            }
        }
        else
        {
            if (m_isButtonPressed)
            {
                m_isButtonPressed = false;
                onClick(m_isButtonPressed);
            }
            if (!m_isHovered)
            {
                m_isHovered = true;
                onHover(m_isHovered);
            }
        }
    }
    else
    {
        if (m_isHovered)
        {
            m_isHovered = false;
            onHover(m_isHovered);
        }
    }
}

// ---------------------------------------------------------

Button::Button(
        sf::Vector2f _position,
        const sf::Texture& _texture,
        const sf::IntRect& _textureRectDefault,
        const sf::IntRect& _textureRectHover,
        ClickCallback _onClickCallback)
    : m_textureRectDefault(_textureRectDefault)
    , m_textureRectHover(_textureRectHover) 
    , m_onClickCallback(_onClickCallback)
{
    m_sprite.setTexture(_texture);
    m_sprite.setTextureRect(_textureRectDefault);
    if (_position.x == CENTER_ALLIGNED) // TODO to fix this
        _position.x = calculateCenterX(m_sprite.getGlobalBounds().width);
    m_sprite.setPosition(_position);
}

sf::FloatRect Button::getBounds() const
{
    return m_sprite.getGlobalBounds();
}

void Button::onUpdate()
{
    Interactable::onUpdate();
}

void Button::onDraw(sf::RenderWindow& _window)
{
    _window.draw(m_sprite);
}

void Button::onHover(bool _isHovered)
{
    if (_isHovered)
        m_sprite.setTextureRect(m_textureRectHover);
    else
        m_sprite.setTextureRect(m_textureRectDefault);
}

void Button::onClick(bool _isButtonPressed) 
{
    if (!_isButtonPressed)
        m_onClickCallback();
}

// ---------------------------------------------------------

ChoosableButton::ChoosableButton(
        sf::Vector2f _position,
        const sf::Texture& _texture,
        const sf::IntRect& _textureRectDefault,
        const sf::IntRect& _textureRectChoose,
        ChooseCallback _onChooseCallback)
    : Button(_position, _texture, _textureRectDefault, _textureRectDefault, {})
    , m_textureRectChoose(_textureRectChoose)
    , m_onChooseCallback(_onChooseCallback)
{
}

void ChoosableButton::unchoose()
{
    if (!m_isChosen)
        return;
    m_isChosen = false;
    onChoose();
}

void ChoosableButton::onClick(bool _isButtonPressed)
{
    if (_isButtonPressed)
        return;
    m_isChosen = !m_isChosen;
    onChoose();
}

void ChoosableButton::onChoose()
{
    if (m_isChosen)
        m_sprite.setTextureRect(m_textureRectChoose);
    else
        m_sprite.setTextureRect(m_textureRectDefault);
    m_onChooseCallback(*this, m_isChosen);
}

// ---------------------------------------------------------

InputField::InputField(
        sf::Vector2f _position,
        const sf::Texture& _texture,
        const sf::Font& _font,
        const std::string& _helpText,
        ValidateEnteredTextCallback _onValidateEnteredTextCallback,
        FinishEnterTextCallback _onFinishEnterTextCallback)
    : m_onValidateEnteredTextCallback(_onValidateEnteredTextCallback)
    , m_onFinishEnterTextCallback(_onFinishEnterTextCallback)
{
    m_sprite.setTexture(_texture);
    if (_position.x == CENTER_ALLIGNED) // TODO to fix this
        _position.x = calculateCenterX(m_sprite.getGlobalBounds().width);
    
    m_sprite.setPosition(_position);

    m_inputText.setFont(_font);
    m_inputText.setString(_helpText);
    m_inputText.setFillColor(sf::Color::Black);
    m_inputText.setPosition(
        _position.x + INPUT_FIELD_MARGIN,
        (m_sprite.getGlobalBounds().height / 2.f - m_inputText.getGlobalBounds().height / 2.f) + _position.y);

}

sf::FloatRect InputField::getBounds() const
{
    return m_sprite.getGlobalBounds();
}

void InputField::onUpdate()
{
    const auto& window = Game::Get().GetWindow();
    if (!window.hasFocus())    
        return;

    Interactable::onUpdate();

    const sf::Vector2f mousePosition = sf::Vector2f(sf::Mouse::getPosition(window));
    if (!getBounds().contains(mousePosition))
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            if (m_isInInputMode)
            {
                m_isInInputMode = false;
                m_onFinishEnterTextCallback(m_enteredStr);
            }
        }
    }

    if (!m_isInInputMode)
        return;

    const sf::Uint32 enteredChar = Game::Get().GetEnteredChar();
    if (enteredChar)
    {
        if (m_onValidateEnteredTextCallback(enteredChar))
        {
            if (enteredChar == 8) // Backspace
            {
                if (!m_enteredStr.empty())
                    m_enteredStr.pop_back();
            }
            else
                m_enteredStr += enteredChar;

            m_inputText.setString(m_enteredStr);
        }
    }
    
    if (Game::Get().isKeyPressed(sf::Keyboard::Enter))
    {
        m_isInInputMode = false;
        m_onFinishEnterTextCallback(m_enteredStr);
    }
}

void InputField::onDraw(sf::RenderWindow& _window)
{
    _window.draw(m_sprite);
    _window.draw(m_inputText);
}

void InputField::onClick(bool _isClicked)
{
    if (!_isClicked)
        m_isInInputMode = true;
}

// ---------------------------------------------------------

SetOfChoosableButtons::SetOfChoosableButtons(
        unsigned _numberOfButtons,
        sf::Vector2f _positionOfTheFirst,
        float _distanceBetweenButtons,
        const sf::Texture& _texture,
        const sf::Vector2i& _buttonDimension,
        ChooseCallback _onChooseCallback)
    : m_onChooseCallback(_onChooseCallback)
{
    m_buttons.reserve(_numberOfButtons);
    for (unsigned num = 0; num < _numberOfButtons; num++)
        m_buttons.emplace_back(ChoosableButton(
            sf::Vector2f(
                _positionOfTheFirst.x 
                    + num * (_distanceBetweenButtons + _buttonDimension.x),
                 _positionOfTheFirst.y),
            _texture,
            sf::IntRect{
                (int)num * _buttonDimension.x,
                0,
                _buttonDimension.x,
                _buttonDimension.y},
            sf::IntRect{
                (int)num * _buttonDimension.x,
                _buttonDimension.y,
                _buttonDimension.x,
                _buttonDimension.y},
            [this, num](ChoosableButton& _button, bool _isChosen){
                onColorButtonChosen(_button, _isChosen, num);
            }
        ));
}

void SetOfChoosableButtons::onUpdate()
{
    for(auto& button : m_buttons)
        button.Update();
}

void SetOfChoosableButtons::onDraw(sf::RenderWindow& _window)
{
    for(auto& button : m_buttons)
        button.Draw(_window);
}

void SetOfChoosableButtons::onColorButtonChosen(ChoosableButton& _button, bool _isChosen, unsigned _num)
{
    if (m_chosenColorButton)
        m_chosenColorButton->unchoose();
    m_onChooseCallback(_num, _isChosen);
    m_chosenColorButton = (_isChosen) ?  &_button : nullptr;
}