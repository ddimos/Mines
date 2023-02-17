#pragma once
#include <SFML/Graphics.hpp>

constexpr float CELL_SIZE = 32.f;
constexpr float OFFSET_BETWEEN_CELLS = 3.f;
const sf::Vector2u WINDOWS_SIZE = { 1024, 768 };
const sf::Vector2u GAME_FIELD_SIZE = { 768, 768 };
const sf::Vector2u INFO_FIELD_SIZE = { 256, 768 };
const float WINDOWS_RATIO = WINDOWS_SIZE.y / (float)WINDOWS_SIZE.x;

constexpr int MAX_PLAYER_COUNT = 8;
constexpr int CAMERA_OFFSET = 2;

constexpr unsigned DEFAULT_MAX_WORLD_WIDTH = 99;
constexpr unsigned DEFAULT_MAX_WORLD_HEIGHT = 99;
constexpr unsigned DEFAULT_MAX_WORLD_BOMBS_COUNT = 99;
constexpr unsigned DEFAULT_MAX_INPUT_FIELD_SIZE = 32;

constexpr float EPSILON = 0.0001f; 

const int InfiniteRadius = -1;

const sf::Color BACKGROUND_COLOR(26, 25, 46);

// Network
const unsigned short DEFAULT_PORT = 20087;

// Menu
constexpr int DEFAULT_TEXT_SIZE = 24; 