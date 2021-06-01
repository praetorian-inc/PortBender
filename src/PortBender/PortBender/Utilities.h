#pragma once

#include <string>

// Needed to prevent the compiler from complaining about "depreciated"
// inet_* functions
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <ws2tcpip.h>
#include <Windows.h>

class Utilities {
public:
	static std::string AddressToString(UINT32 addr);
	static UINT32 StringToAddress(const char* str);
};