#pragma once

#include <string>

class Arguments {
public:
	Arguments(std::string argument_string);
	std::string Action;
	short FakeDstPort;
	short RedirectPort;
	std::string Password;
};