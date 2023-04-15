#pragma once
#include <random>

class Random
{
public:
    Random() = default;
    ~Random() = default;

    void Init(unsigned _seed);
    int GetRand(int _min, int _max);    // [_min, _max) _max is not included

private:
    std::mt19937 m_generator;
};
