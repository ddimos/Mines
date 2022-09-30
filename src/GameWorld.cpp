#include "GameWorld.h"
#include "Game.h"
#include "Utils.h"
#include "Log.h"

#include <functional>
#include <set>

GameWorld::GameWorld(/* args */)
{
}

GameWorld::~GameWorld()
{
}

void GameWorld::CreateWorld(WorldPosition _worldSize, size_t _bombsNumber)
{
    m_worldSize = _worldSize;
    m_cells.clear();
    createCells(_worldSize);
    generateBombs(_bombsNumber);
    m_camera.OnInit(_worldSize);
    m_characters.reserve(5);
}

void GameWorld::DestroyWorld()
{
    m_characters.clear();
    m_mainCharachter = nullptr;
}

void GameWorld::SpawnCharacter(bool _spawnMaster, bool _canControl, unsigned _id, const CharacterInfo& _info)
{
    m_characters.emplace_back(Character(_spawnMaster, _canControl, _id, _info));
    std::string roleStr = (_spawnMaster) ? "M":"R";
    roleStr += (_canControl) ? "+":"";
    LOG("Spawn " + roleStr + " Id: " + tstr(_id));
    if (_canControl)
    {
        if (m_mainCharachter != nullptr)
            LOG_ERROR("Main charachter is already created");
        
        m_mainCharachter = &m_characters.back();
        m_camera.SetTargetedCharacter(m_mainCharachter);    
    }
}

void GameWorld::Update(float _dt)
{
    for (Character& character : m_characters)
        character.Update(_dt);
    // for (Cell& cell : m_cells)
    //     cell.Update(_dt);

    m_camera.Update(_dt);
}

void GameWorld::Render(sf::RenderWindow& _window)
{
    for (Cell& cell : m_cells)
    {
        cell.Render(_window);
    }
    for (Character& character : m_characters)
        character.Render(_window);
}

Cell& GameWorld::getCell(int _x, int _y)
{
    int index = getIndex(_x, _y);
    // TODO: Check limits
    return m_cells[index];
}

Cell& GameWorld::getCell(WorldPosition _pos)
{
    return getCell(_pos.x, _pos.y);
}

void GameWorld::OnCharacterUncoverCell(WorldPosition _pos, Character& _char)
{
    (void)_char;
    
    if (getCell(_pos).m_state == Cell::State::FLAGGED)
        return;

    if (getCell(_pos).m_type == Cell::ValueType::BOMB)
    {
        Game::Get().OnGameEnded(false);
        return;
    }

    onUncoverCell(_pos);
    
    if (getCell(_pos).m_type == Cell::ValueType::EMPTY)
        uncoverCellsInRadius(_pos, m_mainCharachter->GetUncoverRadius()); // TODO should be a character who uncovers
}

void GameWorld::OnCharacterToggleFlagCell(WorldPosition _pos, Character& _char)
{
    getCell(_pos).OnToggleFlag(_char);
}

void GameWorld::onUncoverCell(WorldPosition _pos)
{
    if (getCell(_pos).IsCovered())
    {
        getCell(_pos).OnUncoverCell();
        m_cellsLeftToUncover--;

        if (m_cellsLeftToUncover <= 0)
            Game::Get().OnGameEnded(true);
    }
}

void GameWorld::createCells(WorldPosition _worldSize)
{
	for (int y = 0; y < _worldSize.y; y++)
		for (int x = 0; x < _worldSize.x; x++)
			m_cells.push_back({WorldPosition{x, y}});

    m_cellsLeftToUncover = _worldSize.x * _worldSize.y - BOMBS_COUNT;
}

void GameWorld::generateBombs(size_t _bombsNumber)
{
    while (_bombsNumber)
	{
		const int X = getRand() % m_worldSize.x;
		const int Y = getRand() % m_worldSize.y;

		if(getCell(X, Y).m_type == Cell::ValueType::BOMB)
			continue;

		getCell(X, Y).SetType(Cell::ValueType::BOMB);

		for (int i = -1; i <= 1; i++)
		{
			const int y = Y + i;
			if (y < 0 || y >= m_worldSize.y)
				continue;

			for (int j = -1; j <= 1; j++)
			{
				if (i == 0 && j == 0)
					continue;

				const int x = X + j;
				if (x < 0 || x >= m_worldSize.x)
					continue;

				if (getCell(x, y).m_type == Cell::ValueType::BOMB)
					continue;
				getCell(x, y).SetType(Cell::ValueType::NUMBER);
				getCell(x, y).IncreaseNumber();
			}
		}
		_bombsNumber--;
	}
}

void GameWorld::uncoverCellsInRadius(WorldPosition _pos, int _radius)
{
    std::vector<WorldPosition> cellsToUncover;
    std::function<bool(WorldPosition)> isInArrayToUncover = [&cellsToUncover] (WorldPosition _pos)
    {
        return std::find_if(cellsToUncover.begin(), cellsToUncover.end(), 
                [_pos](WorldPosition _posInArray) { return _pos == _posInArray; }) 
            != cellsToUncover.end();
    };

    std::function<void(WorldPosition)> checkNeib = [&](WorldPosition _posToCheck)
    {
        if (_radius != InfiniteRadius && (_pos - _posToCheck).getLength() >= _radius)
            return;

        std::function<void(WorldPosition)> checkAxis = [&](WorldPosition _posToCheckAxis)
        {
            if (!isInArrayToUncover(_posToCheckAxis))
            {
                cellsToUncover.push_back(_posToCheckAxis);
                if (getCell(_posToCheckAxis).IsEmpty())
                    checkNeib(_posToCheckAxis);
            }
        };
        std::function<void(WorldPosition)> checkNumber = [&](WorldPosition _posToCheckNum)
        {
            if (getCell(_posToCheckNum).IsNumber())
            {
                WorldPosition left {_posToCheckNum.x - 1, _posToCheckNum.y};
                if (left.x >= 0)
                    if (getCell(left).IsNumber() && !isInArrayToUncover(left))
                        cellsToUncover.push_back(left);
                
                WorldPosition right {_posToCheckNum.x + 1, _posToCheckNum.y};
                if (right.x < m_worldSize.x)
                    if (getCell(right).IsNumber() && !isInArrayToUncover(right))
                        cellsToUncover.push_back(right);
            }
        };


        WorldPosition leftX {_posToCheck.x - 1, _posToCheck.y };
        if (leftX.x >= 0)
            checkAxis(leftX);

        WorldPosition rightX {_posToCheck.x + 1, _posToCheck.y};
        if (rightX.x < m_worldSize.x)
            checkAxis(rightX);

        WorldPosition upY {_posToCheck.x, _posToCheck.y - 1};
        if	(upY.y >= 0)
        {
            checkAxis(upY);
            checkNumber(upY);
        }
        WorldPosition downY {_posToCheck.x, _posToCheck.y + 1};
        if	(downY.y < m_worldSize.y)
        {
            checkAxis(downY);
            checkNumber(downY);
        }
    };

    checkNeib(_pos);

    for (WorldPosition pos : cellsToUncover)
        onUncoverCell(pos);
}

unsigned GameWorld::GenerateId()
{
    return m_characters.size() + 1; // TODO: a real bad way to generate ids
}

sf::Color GameWorld::GenerateColor()
{
    const int red = getRand() % 255;
    const int green = getRand() % 255;
    const int blue = getRand() % 255;
    return sf::Color(red, green, blue);
    // static std::vector<sf::Color> colors;
    // colors.push_back(sf::Color::Yellow);
    // colors.push_back(sf::Color::Red);
    // colors.push_back(sf::Color::Magenta);
    // return colors[m_characters.size() + 1];
}

void GameWorld::OnReplicateCharacterControlsMessageReceived(NetworkMessage& _message)
{
    unsigned id;
    _message.Read(id);
    for (Character& charac : m_characters)
    {
        if (charac.GetId() == id)
        {
            charac.OnReplicateCharacterControlsMessageReceived(_message);
            break;
        }
    }
}

void GameWorld::OnReplicateCharacterMessageReceived(NetworkMessage& _message)
{
    unsigned id;
    _message.Read(id);
    for (Character& charac : m_characters)
    {
        if (charac.GetId() == id)
        {
            charac.OnReplicateCharacterMessageReceived(_message);
            break;
        }
    }
}

void GameWorld::OnReplicateUncoverCellMessageReceived(NetworkMessage& _message)
{
    unsigned id;
    _message.Read(id);
    for (Character& charac : m_characters)
    {
        if (charac.GetId() == id)
        {
            charac.OnReplicateUncoverCellMessageReceived(_message);
            break;
        }
    }
}

void GameWorld::OnReplicateToggleFlagCellMessageReceived(NetworkMessage& _message)
{
    unsigned id;
    _message.Read(id);
    for (Character& charac : m_characters)
    {
        if (charac.GetId() == id)
        {
            charac.OnReplicateToggleFlagCellMessageReceived(_message);
            break;
        }
    }
}
