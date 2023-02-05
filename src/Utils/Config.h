#pragma once
#include <string>
#include <unordered_map>
#include <type_traits>

class Config
{
public:
    static bool StartUp(const std::string& _filePath);
    static bool ShutDown();
    static Config& Get() {return *ms_config; }

    template<typename T>
    bool GetConfig(const std::string& _key, T& _outValue)
    {
        static_assert(std::is_integral<T>::value, "Non integral types aren't supported");

        auto it = m_configs.find(toLower(_key));
        if (it == m_configs.end())
            return false;
        
        _outValue = static_cast<T>(it->second);
        return true;
    }

    template<typename T>
    T GetConfig(const std::string& _key, const T& _fallback)
    {
        T value;
        if (!GetConfig(_key, value))
            value = _fallback;
        return value;
    }

private:
    Config(const std::string& _filePath);
    ~Config() = default;
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    std::string toLower(std::string _str) const;
    void loadAndParseFile(const std::string& _filePath);
    
    static Config* ms_config;

    std::unordered_map<std::string, size_t> m_configs;
};