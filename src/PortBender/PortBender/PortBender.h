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