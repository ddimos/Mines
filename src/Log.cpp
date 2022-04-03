#include <iostream>
#include "Log.h"


void Logger::Log(std::string const& message, std::string const& func, int line, Type type)
{
    switch (type)
    {
    case Type::INFO:
    {
        std::cout << "[ "<< func << " ] [ " << line << " ] " << message << std::endl;
        break;
    }
    case Type::ERROR:
    {
        std::cout << "-----ERROR----- [ "<< func << " ] [ " << line << " ] " << message << std::endl;
        break;
    }
    default:
        break;
    }
    
}