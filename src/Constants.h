#pragma once
#include <SFML/Graphics.hpp>

const float CELL_SIZE = 25.f;
const float OFFSET_BETWEEN_CELLS = 3.f;
const sf::Vector2u WINDOWS_SIZE = { 1000, 800 };

const int CELL_COUNT = 15;
const int CAMERA_OFFSET = 2;
const int BOMBS_COUNT = 10;

constexpr unsigned DEFAULT_MAX_WORLD_WIDTH = 99;
constexpr unsigned DEFAULT_MAX_WORLD_HEIGHT = 99;
constexpr unsigned DEFAULT_MAX_WORLD_BOMBS_COUNT = 99;
constexpr unsigned DEFAULT_MAX_INPUT_FIELD_SIZE = 32;

const float EPSILON = 0.0001f; 

const int InfiniteRadius = -1;

const sf::Color BACKGROUND_COLOR(26, 25, 46);

// Network
const unsigned short DEFAULT_PORT = 20087;

// Menu
constexpr int DEFAULT_TEXT_SIZE = 24; 