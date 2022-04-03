#pragma once

#include <string>

#define LOG(mes) \
Logger::Log(mes, __FUNCTION__, __LINE__)

#define LOG_ERROR(mes) \
Logger::Log(mes, __FUNCTION__, __LINE__, Logger::Type::ERROR)

#define tstr std::to_string

class Logger
{
public:
    enum class Type
    {
        NONE = 0,
        INFO = 1,
        ERROR = 2
    };

    static void Log(std::string const& message, std::string const& func, int line, Type type=Type::INFO);
};

