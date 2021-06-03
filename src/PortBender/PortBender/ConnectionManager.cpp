// Copyright 2021 Praetorian Security, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ConnectionManager.h"

ConnectionManager::ConnectionManager() {
	this->connections = std::map<std::string, Connection*>();
	this->clients = std::vector<std::string>();
}

void ConnectionManager::Add(Connection* connection)
{
	std::string key = Utilities::AddressToString(connection->ip) + ":" + std::to_string(ntohs(connection->port));
	std::pair<std::string, Connection*> entry = std::pair<std::string, Connection*>(key, connection);
	this->connections.insert(entry);
}

void ConnectionManager::AddBackdoorClient(std::string ip)
{
	if (std::find(clients.begin(), clients.end(), ip) == clients.end())
	{
		clients.push_back(ip);
	}
}

ConnectionManager::ConnectionType ConnectionManager::GetConnectionState(Packet* packet)
{
	if (packet->tcp_header->Syn && !packet->tcp_header->Ack) {
		printf("New connection from %s:%d to %s:%d\n", Utilities::AddressToString(packet->ip_header->SrcAddr).c_str(),
			ntohs(packet->tcp_header->SrcPort),
			Utilities::AddressToString(packet->ip_header->DstAddr).c_str(),
			ntohs(packet->tcp_header->DstPort));

		return ConnectionManager::ConnectionType::CONNECT;
	} else if (packet->tcp_header->Rst) {
		printf("Disconnect from %s:%d to %s:%d\n", Utilities::AddressToString(packet->ip_header->SrcAddr).c_str(),
			ntohs(packet->tcp_header->SrcPort),
			Utilities::AddressToString(packet->ip_header->DstAddr).c_str(),
			ntohs(packet->tcp_header->DstPort));

		return ConnectionManager::ConnectionType::DISCONNECT;
	} else {
		return ConnectionManager::ConnectionType::ESTABLISHED;
	}
}

BOOL ConnectionManager::IsBackdoorClient(Packet* packet)
{
	std::string ip1 = Utilities::AddressToString(packet->ip_header->SrcAddr);
	std::string ip2 = Utilities::AddressToString(packet->ip_header->DstAddr);

	if (std::find(clients.begin(), clients.end(), ip1) != clients.end()) {
		return TRUE;
	} else if (std::find(clients.begin(), clients.end(), ip2) != clients.end()) {
		return TRUE;
	} else {
		return FALSE;
	}
}

BOOL ConnectionManager::IsRedirectedConnection(Packet* packet) {
	ConnectionType state = this->GetConnectionState(packet);

	if (state == ConnectionType::CONNECT) {
		Connection* connection = new Connection(packet->ip_header->SrcAddr,
			packet->tcp_header->SrcPort);
		this->Add(connection);
		return TRUE;
	}
	else {
		Connection* connection = this->Lookup(packet);

		if (connection != NULL) {
			if (state == ConnectionType::DISCONNECT) {
				this->Remove(connection);
			}
			return TRUE;
		}
		else {
			return FALSE;
		}
	}
}

Connection* ConnectionManager::Lookup(Packet *packet)
{
	//
	// The key is either doing to be the destination ip/port or source ip/port depending
	// on if this an inbound or outbound connection
	//

	std::string key1 = Utilities::AddressToString(packet->ip_header->SrcAddr) + ":" + std::to_string(ntohs(packet->tcp_header->SrcPort));
	std::string key2 = Utilities::AddressToString(packet->ip_header->DstAddr) + ":" + std::to_string(ntohs(packet->tcp_header->DstPort));

	//
	// Attempt to lookup the key using source ip/port
	//

	std::map<std::string, Connection*>::iterator i = this->connections.find(key1);
	if (i != this->connections.end()) {
		return i->second;
	}

	//
	// Attempt to lookup the key using destination ip/port
	//

	i = this->connections.find(key2);
	if (i != this->connections.end()) {
		return i->second;
	}
	
	//
	// Unable to lookup the connection return null instead
	//

	return NULL;
}

void ConnectionManager::Remove(Connection* connection)
{
	std::string key = Utilities::AddressToString(connection->ip) + ":" + std::to_string(ntohs(connection->port));
	this->connections.erase(key);
}

void ConnectionManager::RemoveBackdoorClient(std::string ip)
{
	clients.erase(std::remove(clients.begin(), clients.end(), ip), clients.end());
}