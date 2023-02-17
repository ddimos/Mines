#include "GameWorld.h"
#include "Game.h"
#include "Utils/Utils.h"
#include "Utils/Log.h"

#include <functional>

GameWorld::GameWorld()
    : m_worldMap(*this)
{
}

void GameWorld::CreateWorld(WorldConfig _worldConfig)
{
    m_worldConfig = _worldConfig;
    m_cells.clear();
    createCells(m_worldConfig.worldSize);
    generateBombs(m_worldConfig.bombsCount);
    m_camera.OnInit(m_worldConfig.worldSize);
    m_characters.reserve(MAX_PLAYER_COUNT);
    m_worldMap.CreateMap(m_worldConfig.worldSize);
}

void GameWorld::DestroyWorld()
{
    m_characters.clear();
    m_mainCharachter = nullptr;
}

void GameWorld::SpawnCharacter(bool _spawnMaster, bool _canControl, const CharacterInfo& _info)
{
    m_characters.emplace_back(Character(*this, _spawnMaster, _canControl, _info));
    std::string roleStr = (_spawnMaster) ? "M":"R";
    roleStr += (_canControl) ? "+":"";
    LOG("Spawn " + roleStr + " Id: " + tstr(_info.characterId));
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

    m_camera.Update(_dt);
}

void GameWorld::Render(sf::RenderWindow& _window)
{
    _window.draw(m_worldMap);

    for (Character& character : m_characters)
        character.Render(_window);
}

Cell& GameWorld::getCell(int _x, int _y)
{
    int index = getCellIndex(_x, _y);
    // TODO: Check limits
    return m_cells[index];
}

Cell& GameWorld::getCell(WorldPosition _pos)
{
    return getCell(_pos.x, _pos.y);
}

const Character& GameWorld::GetCharacter(CharacterID _id) const
{
    auto it = std::find_if(m_characters.begin(), m_characters.end(),
        [_id](const Character& _c){ return _c.GetId() == _id; });
    return *it;
}

void GameWorld::OnCharacterUncoverCell(WorldPosition _pos, Character& _char)
{
    (void)_char;
    
    if (getCell(_pos).m_state == Cell::State::FLAGGED)
        return;

    if (getCell(_pos).m_type == Cell::ValueType::BOMB)
    {
        onUncoverCell(_pos);
        Game::Get().OnGameEnded(false, _char.GetInfo().playerId);
        return;
    }

    onUncoverCell(_pos);
    
    if (getCell(_pos).m_type == Cell::ValueType::EMPTY)
        uncoverCellsInRadius(_pos, m_mainCharachter->GetUncoverRadius()); // TODO should be a character who uncovers

    if (m_cellsLeftToUncover <= 0)
        Game::Get().OnGameEnded(true);
}

void GameWorld::OnCharacterToggleFlagCell(WorldPosition _pos, Character& _char)
{
    (void)_char;

    auto& cell = getCell(_pos);
    cell.ToggleFlag(_char);
    m_worldMap.OnToggleFlag(cell);
    Game::Get().OnCharacterToggleFlagCell(cell, _char);
}

void GameWorld::onUncoverCell(WorldPosition _pos)
{
    if (getCell(_pos).IsCovered())
    {
        getCell(_pos).Uncover();
        m_worldMap.OnUncoverCell(getCell(_pos));
        m_cellsLeftToUncover--;
    }
}

void GameWorld::createCells(WorldPosition _worldSize)
{
    m_cells.reserve(_worldSize.x*_worldSize.y);
	for (int y = 0; y < _worldSize.y; y++)
		for (int x = 0; x < _worldSize.x; x++)
			m_cells.push_back({WorldPosition{x, y}});

    m_cellsLeftToUncover = _worldSize.x * _worldSize.y - m_worldConfig.bombsCount;
}

void GameWorld::generateBombs(size_t _bombsNumber)
{
    while (_bombsNumber)
	{
		const int X = getRand() % m_worldConfig.worldSize.x;
		const int Y = getRand() % m_worldConfig.worldSize.y;

		if(getCell(X, Y).m_type == Cell::ValueType::BOMB)
			continue;

		getCell(X, Y).setType(Cell::ValueType::BOMB);

		for (int i = -1; i <= 1; i++)
		{
			const int y = Y + i;
			if (y < 0 || y >= m_worldConfig.worldSize.y)
				continue;

			for (int j = -1; j <= 1; j++)
			{
				if (i == 0 && j == 0)
					continue;

				const int x = X + j;
				if (x < 0 || x >= m_worldConfig.worldSize.x)
					continue;

				if (getCell(x, y).m_type == Cell::ValueType::BOMB)
					continue;
				getCell(x, y).setType(Cell::ValueType::NUMBER);
				getCell(x, y).increaseNumber();
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
                if (right.x < m_worldConfig.worldSize.x)
                    if (getCell(right).IsNumber() && !isInArrayToUncover(right))
                        cellsToUncover.push_back(right);
            }
        };


        WorldPosition leftX {_posToCheck.x - 1, _posToCheck.y };
        if (leftX.x >= 0)
            checkAxis(leftX);

        WorldPosition rightX {_posToCheck.x + 1, _posToCheck.y};
        if (rightX.x < m_worldConfig.worldSize.x)
            checkAxis(rightX);

        WorldPosition upY {_posToCheck.x, _posToCheck.y - 1};
        if	(upY.y >= 0)
        {
            checkAxis(upY);
            checkNumber(upY);
        }
        WorldPosition downY {_posToCheck.x, _posToCheck.y + 1};
        if	(downY.y < m_worldConfig.worldSize.y)
        {
            checkAxis(downY);
            checkNumber(downY);
        }
    };

    checkNeib(_pos);

    for (WorldPosition pos : cellsToUncover)
        onUncoverCell(pos);
}

int GameWorld::getCellIndex(int _x, int _y)
{
    return _x + _y * (m_worldConfig.worldSize.x);
}

void GameWorld::OnReplicateCharacterControlsMessageReceived(NetworkMessage& _message)
{
    CharacterID id;
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
    CharacterID id;
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
    CharacterID id;
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
    CharacterID id;
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
