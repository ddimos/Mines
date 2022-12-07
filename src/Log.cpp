#include <iostream>
#include "Log.h"

Logger::Type Logger::ms_logLevel = Logger::Type::INFO;

void Logger::Log(std::string const& _message, std::string const& _func, int _line, Type _type)
{
    if (ms_logLevel == Type::NONE || ms_logLevel > _type)
        return;

    switch (_type)
    {
    case Type::DEBUG:
    case Type::INFO:
    {
        std::cout << "[ "<< _func << " ] [ " << _line << " ] " << _message <<  "\n";
        break;
    }
    case Type::ERROR:
    {
        std::cout << "-----ERROR----- [ "<< _func << " ] [ " << _line << " ] " << _message << std::endl;
        break;
    }
    default:
        break;
    }
}