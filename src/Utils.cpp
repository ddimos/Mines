#include "Utils.h"

void provideSeed(unsigned _seed)
{
    srand(_seed);
}

int getRand()
{
    return rand();
}
