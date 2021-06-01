#pragma once

#include <Windows.h>

class Connection {
public:
	Connection(UINT32 ip, UINT16 port);

	UINT32 ip;
	UINT16 port;
};