#include <iostream>
#include <fstream>
#include <unordered_map>
#include <chrono>
#include <iomanip>

#include "Utils/Log.h"
#include "Utils/Config.h"

namespace
{
    std::unordered_map<Logger::Type, Logger::Level> logLevels;
    const std::unordered_map<Logger::Type, std::string> logTypesStr{
        {Logger::Type::SYSTEM,  " [system] "},
        {Logger::Type::GAME,    " [game] "},
        {Logger::Type::NETWORK, " [network] "},
        {Logger::Type::MENU,    " [menu] "}
    };

    std::ofstream ofs;
} // namespace

void Logger::StartUp()
{
    logLevels={
        { Logger::Type::SYSTEM,     static_cast<Logger::Level>(Config::Get().GetConfig("logLevelSystem", 1))},
        { Logger::Type::GAME,       static_cast<Logger::Level>(Config::Get().GetConfig("logLevelGame", 1))},
        { Logger::Type::NETWORK,    static_cast<Logger::Level>(Config::Get().GetConfig("logLevelNetwork", 1))},
        { Logger::Type::MENU,       static_cast<Logger::Level>(Config::Get().GetConfig("logLevelMenu", 1))}
    };
    
    if (Config::Get().GetConfig("logToFile", 0))
    {
        ofs.open("log.log");
        std::cout.rdbuf(ofs.rdbuf());
    }
}

void Logger::Log(Level _level, Type _type, std::string_view _message)
{
    if (logLevels[_type] < _level)
        return;

    auto it = logTypesStr.find(_type);
    std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    switch (_level)
    {
    case Level::DEBUG:
    case Level::INFO:
    {
        std::cout << std::put_time(std::gmtime(&time), "%F %T ") << it->second << _message << "\n";
        break;
    }
    case Level::ERROR:
    {
        std::cout << std::put_time(std::gmtime(&time), "%F %T ") << " [ERROR] " << it->second << _message << std::endl;
        break;
    }
    default:
        break;
    }
}