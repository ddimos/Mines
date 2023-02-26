#pragma once

#include <string_view>

#define LOG(type, mes) \
Logger::Log(Logger::Level::INFO, type, mes)

#define LOG_DEBUG(type, mes) \
Logger::Log(Logger::Level::DEBUG, type, mes)

#define LOG_TRACE(type, mes) \
Logger::Log(Logger::Level::TRACE, type, mes)

#define LOG_ERROR(type, mes) \
Logger::Log(Logger::Level::ERROR, type, mes)

#define tstr std::to_string

class Logger
{
public:
    enum class Type
    {
        SYSTEM = 1,
        GAME,
        NETWORK,
        MENU
    };

    enum class Level
    {
        NONE   = 0,
        ERROR  = 1,
        INFO   = 2,
        DEBUG  = 3,
        TRACE  = 4 
    };
    static void StartUp();

    static void Log(Level _level, Type _type, std::string_view _message);
};

