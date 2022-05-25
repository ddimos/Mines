#pragma once

#include <SFML/Network.hpp>
#include <string>

struct NetworkAddress
{
    sf::IpAddress   address = {};
    unsigned short  port = 0;
    std::string toString() const { return address.toString()+":"+std::to_string(port); }
};
inline bool operator==(const NetworkAddress& _lhs, const NetworkAddress& _rhs)
{
    return _lhs.address == _rhs.address && _lhs.port == _rhs.port;
}
inline bool operator!=(const NetworkAddress& _lhs, const NetworkAddress& _rhs)
{
    return !(_lhs == _rhs);
}
