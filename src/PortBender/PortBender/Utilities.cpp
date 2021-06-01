#include "Utilities.h"

std::string Utilities::AddressToString(UINT32 value)
{
	struct in_addr addr  = { 0 };
	addr.S_un.S_addr = value;
	return inet_ntoa(addr);
}

UINT32 Utilities::StringToAddress(const char* str)
{
	struct in_addr addr = { 0 };
	inet_pton(AF_INET, str, &addr);
	return addr.S_un.S_addr;
}