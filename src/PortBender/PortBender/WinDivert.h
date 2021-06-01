#pragma once

#include <Windows.h>

class WinDivert {
public:
	WinDivert(char *filter);
	Packet* Receive();
	void Send(PVOID data, DWORD length, WINDIVERT_ADDRESS addr);
private:
	HANDLE handle = NULL;
};