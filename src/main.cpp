
#if 0

#include <SFML/Graphics.hpp>
#include <vector>
#include <set>
#include <iostream>
#include <functional>

const unsigned int WINDOW_WIDTH = 600;
const unsigned int WINDOW_HEIGHT = 600;

const float CELL_SIZE = 50.f;
const int CELL_COUNT = 10;
const int BOMBS_COUNT = 10;

int getElementIndex(int _x, int _y)
{
	return _x + _y * CELL_COUNT;
}

//---------------- CELL -------------------------
//states: uncovered, covered and flagged.
class Cell
{
public:
	enum class ValueType
	{
		EMPTY,
		BOMB,
		NUMBER
	};

    enum class State
    {
        COVERED,
        UNCOVERED,
        FLAGGED
    };

	Cell(sf::Vector2f _position, const sf::Font& _font)
	: m_position{_position}
	{
		m_shape.setPosition(_position);
		m_shape.setFillColor(sf::Color::Green);
		m_shape.setSize(sf::Vector2f{CELL_SIZE, CELL_SIZE});

        m_flagShape.setPosition(_position);
		m_flagShape.setFillColor(sf::Color::Red);
		m_flagShape.setRadius(CELL_SIZE/3);


		m_text.setPosition(_position);
		m_text.setFont(_font);
		//m_text.setCharacterSize(24); 
		//m_text.setString("H");
		m_text.setFillColor(sf::Color::Black);
		//m_text.setStyle(sf::Text::Bold | sf::Text::Underlined);
	}

	void SetValue(const std::string& _str)
	{
		m_text.setString(_str);
	}

	void SetType(ValueType _type) { m_type = _type;}

	void IncreaseNumber() { m_number++; }

    void ToggleFlag()
    {
        if (m_state == State::UNCOVERED)
            return;

        m_state = (m_state == State::COVERED) ? State::FLAGGED : State::COVERED;
    }

	void Update()
	{
		// Do it once
		if (m_type == ValueType::NUMBER)
			m_text.setString(std::to_string(m_number));

		if (m_isActive)
			m_shape.setFillColor(sf::Color::Yellow);
		else
			if (m_state == State::UNCOVERED)	
				if (m_type == ValueType::BOMB)
					m_shape.setFillColor(sf::Color::Red);
				else if (m_type == ValueType::NUMBER)
					m_shape.setFillColor(sf::Color::Blue);
				else
					m_shape.setFillColor(sf::Color::Cyan);
			else
				m_shape.setFillColor(sf::Color::Green);

		
		
	}

	void Render(sf::RenderWindow& _window)
	{
		_window.draw(m_shape);
		// if (m_isOpen)
        if (m_state == State::UNCOVERED && m_type != ValueType::EMPTY)
        {
			_window.draw(m_text);
        }
        if (m_state == State::FLAGGED)
        {
			_window.draw(m_flagShape);
        }
        
	}

	sf::Vector2f m_position;
	sf::RectangleShape m_shape;//(sf::Vector2f(120.f, 50.f));
	sf::CircleShape m_flagShape;
	sf::Text m_text;

	bool m_isActive = false;
//	bool m_isOpen = false;
    State m_state = State::COVERED;
	ValueType m_type = ValueType::EMPTY;
	int m_number = 0;


};

//---------------- GENERATE BOMS -------------------------

void generateBombs(unsigned _bombsNum, std::vector<Cell>& _cells, int _cursorX, int _cursorY)
{
	srand (time(NULL));

	while (_bombsNum)
	{
		int X = rand() % CELL_COUNT; 
		int Y = rand() % CELL_COUNT; 
		
		if (_cursorX == X && _cursorY == Y)
			continue;

		int pos = X + Y * CELL_COUNT;
		if(_cells[pos].m_type == Cell::ValueType::BOMB)
			continue;

		_cells[pos].SetType(Cell::ValueType::BOMB);
		_cells[pos].SetValue("B");

		for (int i = -1; i <= 1; i++)
		{
			int y = Y + i;
			if (y < 0 || y >= CELL_COUNT)
				continue;
			
			for (int j = -1; j <= 1; j++)
			{
				if (i == 0 && j == 0)
					continue;

				int x = X + j;
				if (x < 0 || x >= CELL_COUNT)
					continue;

				int position = x + y * CELL_COUNT;
				if (_cells[position].m_type == Cell::ValueType::BOMB)
					continue;	 
				_cells[position].SetType(Cell::ValueType::NUMBER);
				_cells[position].IncreaseNumber();
			}
		}
		
		
		

		_bombsNum--;

	}
	
}

//---------------- CURSOR -------------------------

struct Cursor
{
	void Update()
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)
		 || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)
		 || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)
		 || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			if (m_posX == InvalidPos && m_posY == InvalidPos)
			{
				m_isAcive = true;
				m_posX = 0;
				m_posY = 0;
				m_wasLeftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
				m_wasRightPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
				m_wasUpPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
				m_wasDownPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
				return;
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			if (!m_wasLeftPressed)
			{
				m_posX--;
				m_wasLeftPressed = true;
			}
		}
		else
		{
			m_wasLeftPressed = false;
		}
		
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			if (!m_wasRightPressed)
			{
				m_posX++;
				m_wasRightPressed = true;
			}
		}
		else
		{
			m_wasRightPressed = false;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			if (!m_wasUpPressed)
			{
				m_posY--;
				m_wasUpPressed = true;
			}
		}
		else
		{
			m_wasUpPressed = false;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			if (!m_wasDownPressed)
			{
				m_posY++;
				m_wasDownPressed = true;
			}
		}
		else
		{
			m_wasDownPressed = false;
		}

		if (m_posX == InvalidPos && m_posY == InvalidPos)
			return;

		if (m_posX < 0)
			m_posX = CELL_COUNT - 1;
		else if (m_posX >= CELL_COUNT)
			m_posX = 0;
		
		if (m_posY < 0)
			m_posY = CELL_COUNT - 1;
		else if (m_posY >= CELL_COUNT)
			m_posY = 0;
	}

	static const int InvalidPos = 548654;
	bool m_isAcive = false;
	int m_posX = InvalidPos;
	int m_posY = InvalidPos;

private:
	bool m_wasUpPressed = false;
	bool m_wasDownPressed = false;
	bool m_wasLeftPressed = false;
	bool m_wasRightPressed = false;
};

//---------------- MAIN -------------------------
int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Mines!");

	std::vector<Cell> m_cells;
	Cursor m_cursor;
	int m_currentPos = Cursor::InvalidPos;
	int m_previousPos = Cursor::InvalidPos;
	sf::Font m_font;
	if (!m_font.loadFromFile("res/fonts/times_new_roman.ttf"))
	{
		std::cout << "Couldn't load the font" << std::endl;
	}

	bool m_areBombsGenerated = false;

	//---------------- CREATE -------------------------
	
	const float offset = 5; 
	float nextPosX = 0.f;
	float nextPosY = 0.f;
	for (size_t i = 0; i < CELL_COUNT; i++)
	{
		for (size_t j = 0; j < CELL_COUNT; j++)
		{
			m_cells.push_back({sf::Vector2f{nextPosX, nextPosY}, m_font});
			nextPosX += CELL_SIZE + offset;
		}
		nextPosX = 0.f;
		nextPosY += CELL_SIZE + offset;	
	}
	

	//---------------- MAIN LOOP -------------------------
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

		//---------------- UPDATE -------------------------

		m_cursor.Update();
		//std::cout << 
		if(m_cursor.m_isAcive)
		{

			m_currentPos = m_cursor.m_posX + m_cursor.m_posY * CELL_COUNT;
			m_cells[m_currentPos].m_isActive = true;
			if (m_previousPos != Cursor::InvalidPos && m_previousPos != m_currentPos)
				m_cells[m_previousPos].m_isActive = false;
			
            static bool zWasPressed = false;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
            {
                if (!zWasPressed)
                {
                    m_cells[m_currentPos].ToggleFlag();
                    zWasPressed = true;
                }
            }
            else
            {
                zWasPressed = false;
            }
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
			{
				if (!m_areBombsGenerated)
				{
					generateBombs(BOMBS_COUNT, m_cells, m_cursor.m_posX, m_cursor.m_posY);
					m_areBombsGenerated = true;
				}

                if (m_cells[m_currentPos].m_state != Cell::State::FLAGGED)
                {
                    if (m_cells[m_currentPos].m_type == Cell::ValueType::NUMBER)
                    {
                        /* code */
                    m_cells[m_currentPos].m_state = Cell::State::UNCOVERED;
                    }
                    else if (m_cells[m_currentPos].m_type == Cell::ValueType::EMPTY)
                    {
                        m_cells[m_currentPos].m_state = Cell::State::UNCOVERED;

                        std::function<void(int, int)> checkNeib = [&](int _posToCheckX, int _posToCheckY)
                        {
                            int leftX = _posToCheckX - 1;
                            if	(leftX >= 0)
                            { 
                                int pos = getElementIndex(leftX, _posToCheckY);
                                if (m_cells[pos].m_state == Cell::State::COVERED)
                                {
                                    m_cells[pos].m_state = Cell::State::UNCOVERED;
                                    if (m_cells[pos].m_type == Cell::ValueType::EMPTY)
                                        checkNeib(leftX, _posToCheckY);
                                }
                            }
                            int rightX = _posToCheckX + 1;
                            if	(rightX < CELL_COUNT)
                            { 
                                int pos = getElementIndex(rightX, _posToCheckY);
                                if (m_cells[pos].m_state == Cell::State::COVERED)
                                {
                                    m_cells[pos].m_state = Cell::State::UNCOVERED;
                                    if (m_cells[pos].m_type == Cell::ValueType::EMPTY)
                                        checkNeib(rightX, _posToCheckY);
                                }
                            }
                            int upY = _posToCheckY - 1;
                            if	(upY >= 0)
                            { 
                                int pos = getElementIndex(_posToCheckX, upY);
                                if (m_cells[pos].m_state == Cell::State::COVERED)
                                {
                                    m_cells[pos].m_state = Cell::State::UNCOVERED;
                                    if (m_cells[pos].m_type == Cell::ValueType::EMPTY)
                                        checkNeib(_posToCheckX, upY);
                                }
                                if (m_cells[pos].m_type == Cell::ValueType::NUMBER)
                                {
                                    int downLeftX = _posToCheckX - 1;
                                    if (downLeftX >= 0)
                                    {
                                        int pos1 = getElementIndex(downLeftX, upY);
                                        if (m_cells[pos1].m_type == Cell::ValueType::NUMBER)
                                            m_cells[pos1].m_state = Cell::State::UNCOVERED;
                                    }
                                    int downRightX = _posToCheckX + 1;
                                    if (downRightX < CELL_COUNT)
                                    {
                                        int pos1 = getElementIndex(downRightX, upY);
                                        if (m_cells[pos1].m_type == Cell::ValueType::NUMBER)
                                            m_cells[pos1].m_state = Cell::State::UNCOVERED;
                                    }
                                }
                            }
                            int downY = _posToCheckY + 1;
                            if	(downY < CELL_COUNT)
                            { 
                                int pos = getElementIndex(_posToCheckX, downY);
                                if (m_cells[pos].m_state == Cell::State::COVERED)
                                {
                                    m_cells[pos].m_state = Cell::State::UNCOVERED;
                                    if (m_cells[pos].m_type == Cell::ValueType::EMPTY)
                                        checkNeib(_posToCheckX, downY);
                                }
                                if (m_cells[pos].m_type == Cell::ValueType::NUMBER)
                                {
                                    int downLeftX = _posToCheckX - 1;
                                    if (downLeftX >= 0)
                                    {
                                        int pos1 = getElementIndex(downLeftX, downY);
                                        if (m_cells[pos1].m_type == Cell::ValueType::NUMBER)
                                            m_cells[pos1].m_state = Cell::State::UNCOVERED;
                                    }
                                    int downRightX = _posToCheckX + 1;
                                    if (downRightX < CELL_COUNT)
                                    {
                                        int pos1 = getElementIndex(downRightX, downY);
                                        if (m_cells[pos1].m_type == Cell::ValueType::NUMBER)
                                            m_cells[pos1].m_state = Cell::State::UNCOVERED;
                                    }
                                }
                            }
                        };
                        // for (int cellNum : cellsToOpen)
                        // {
                        // 	m_cells[cellNum].m_isOpen = true;
                        // }
                        
                        // while(true)
                        // {
                        // 	{
                        checkNeib(m_cursor.m_posX, m_cursor.m_posY);

                        // 	}
                        // }
                    }
                }

			}
		}
		m_previousPos = m_currentPos;

		for (Cell& cell : m_cells)
		{
        	cell.Update();	
		}

		//---------------- RENDER -------------------------
        window.clear();
		for (Cell& cell : m_cells)
		{
			cell.Render(window);			
		}
		
        window.display();
    }

    return 0;
}

#else

#include "App.h"
#include <SFML/Graphics.hpp>
#include "Utils.h"

int main()
{
	Application app;
    sf::RenderWindow window(sf::VideoMode(WINDOWS_SIZE.x, WINDOWS_SIZE.y), "Mines");
    app.StartUp(&window);

    app.MainLoop();

    app.ShutDown();
	return 0;
}

#endif