#pragma once
#include <SFML/Graphics.hpp>

const float CELL_SIZE = 25.f;
const float OFFSET_BETWEEN_CELLS = 5.f;
const sf::Vector2u WINDOWS_SIZE = { 1000, 800 };

const int CELL_COUNT = 50;
const int CAMERA_OFFSET = 2;
const int BOMBS_COUNT = 80;

const float EPSILON = 0.0001f; 

const int InfiniteRadius = -1;


// Network
const unsigned short DEFAULT_PORT = 20087;