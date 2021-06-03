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
#include <string>
#include <sstream>
#include <vector>

#include "Arguments.h"

Arguments::Arguments(std::string argument_string) {
    std::string arg;
    std::stringstream ss(argument_string);

    std::vector<std::string> args;
    while (ss >> arg) {
        args.push_back(arg);
    }
	
	if (args[0] == "redirect") {

		if (args.size() != 3) {
			throw std::invalid_argument("Missing required arguments");
		}

		this->Action = args[0];
		this->FakeDstPort = std::stoi(args[1]);
		this->RedirectPort = std::stoi(args[2]);
	} else if (args[0] == "backdoor") {

		if (args.size() != 4) {
			throw std::invalid_argument("Missing required arguments");
		}

		this->Action = args[0];
		this->FakeDstPort = std::stoi(args[1]);
		this->RedirectPort = std::stoi(args[2]);
		this->Password = args[3];
	} else {
		throw std::invalid_argument("Please enter a valid action of redirect or backdoor");
	}
}