#pragma once

#include <string>

#define LOG(mes) \
Logger::Log(mes, __FUNCTION__, __LINE__)

#define LOG_DEBUG(mes) \
Logger::Log(mes, __FUNCTION__, __LINE__, Logger::Type::DEBUG)

#define LOG_ERROR(mes) \
Logger::Log(mes, __FUNCTION__, __LINE__, Logger::Type::ERROR)

#define tstr std::to_string

class Logger
{
public:
    enum class Type
    {
        DEBUG = 1,
        INFO = 2,
        ERROR = 3,
        NONE = 4
    };

    static void Log(std::string const& _message, std::string const& _func, int _line, Type _type=Type::INFO);

private:
    static Type ms_logLevel;
};

