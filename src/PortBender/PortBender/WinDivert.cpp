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

#include <iostream>
#include <stdio.h>

#include "Packet.h"
#include "WinDivert.h"

#include "libwindivert.h"

WinDivert::WinDivert(char *filter) {
	this->handle = WinDivertOpen(filter, WINDIVERT_LAYER_NETWORK, 0, 0);
	if (this->handle == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_INVALID_PARAMETER)
		{
			std::cout << "Error invalid filter syntax was used" << std::endl;
			std::cout << std::flush;
			ExitProcess(0);
		}
		else if (GetLastError() == ERROR_ACCESS_DENIED) {

		}
		std::cout << "Failed to open the WinDivert device (" << GetLastError() << ")" << std::endl;
		std::cout << std::flush;
		ExitProcess(0);
	}

	if (!WinDivertSetParam(this->handle, WINDIVERT_PARAM_QUEUE_LEN, 8192)) {
		std::cout << "Failed to set packet queue length (" << GetLastError() << ")" << std::endl;
		std::cout << std::flush;
		ExitProcess(0);
	}

	if (!WinDivertSetParam(this->handle, WINDIVERT_PARAM_QUEUE_TIME, 2048)) {
		std::cout << "Failed to set packet queue time (" << GetLastError << ")" << std::endl;
		std::cout << std::flush;
		ExitProcess(0);
	}
}

Packet* WinDivert::Receive() {
	Packet *packet = new Packet();

	if (!WinDivertRecv(this->handle, packet->packet, sizeof(packet->packet), 
		               &packet->addr, &packet->packet_len)) {
		std::cout << "Failed to read packet (" << GetLastError() << ")" << std::endl;;
		std::cout << std::flush;
		ExitProcess(0);
	}

	WinDivertHelperParsePacket(packet->packet, packet->packet_len, 
		                       &packet->ip_header, &packet->ipv6_header, 
		                       &packet->icmp_header, &packet->icmpv6_header, 
		                       &packet->tcp_header, &packet->udp_header, 
		                       &packet->payload, &packet->payload_len);

	if (packet->tcp_header == NULL) {
		std::cout << "Error this shouldn't happen" << std::endl;
		std::cout << std::flush;
		ExitProcess(0);
	}

	return packet;
}

void WinDivert::Send(PVOID data, DWORD length, WINDIVERT_ADDRESS addr) {

	if (!WinDivertSend(this->handle, (PVOID)data, length, &addr, NULL))
	{
		std::cout << "Failed to reinject packet (" << GetLastError() << ")" << std::endl;
		std::cout << std::flush;
		ExitProcess(0);
	}
}