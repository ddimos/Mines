#pragma once
#include "NetworkAddress.h"
#include "NetworkMessage.h"
#include "NetworkPlayer.h"

#include <string>

class SessionManager
{

public:
    SessionManager(/* args */);
    ~SessionManager();

        void CreateSession();
    void JoinSession(NetworkAddress _address, const std::string& _name);

  


private:
    
    // std::vector<NetworkPlayer> m_players; // TODO sharedPtr

    // bool m_isSessionMaster = false;
    // bool m_isSessionCreated = false;

    // PlayerID m_playerIdGenerator = 0;

    // std::string m_pendingPlayerToJoin;
};

SessionManager::SessionManager(/* args */)
{
}

SessionManager::~SessionManager()
{
}
