#include "Utils/Config.h"
#include "Utils/Log.h"
#include <algorithm>
#include <fstream>
#include <sstream>

Config* Config::ms_config = nullptr;

bool Config::StartUp(const std::string& _filePath)
{
    if (!ms_config)
    {
        ms_config = new Config(_filePath);
        return true;
    }
    return false;
}

bool Config::ShutDown()
{
    if (ms_config)
    {
        delete ms_config;
        ms_config = nullptr;
        return true;
    }
    return false;
}

Config::Config(const std::string& _filePath)
{
    loadAndParseFile(_filePath);
}

void Config::loadAndParseFile(const std::string& _filePath)
{
    std::ifstream readFile(_filePath);
    if (!readFile.is_open())
    {
        LOG_ERROR("Cannot open the config file");
        return;
    }

    std::string line,key;
    size_t value;
    while(getline(readFile,line))
    {
        std::stringstream ss(line);
        getline(ss, key, '=');
        key.erase(remove_if(key.begin(), key.end(), isspace), key.end());
        ss >> value;
        m_configs.insert_or_assign(toLower(key), value);
    }
}

std::string Config::toLower(std::string _str) const
{
    std::transform(_str.begin(), _str.end(), _str.begin(), ::tolower);
    return _str;
}
