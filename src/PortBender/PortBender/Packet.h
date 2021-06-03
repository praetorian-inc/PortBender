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

#include "libwindivert.h"

#define MAXBUF  0xFFFF

//
// Disable Warnings about Zero-Length Array Definitions
//
#pragma warning(disable : 4200)

//
// Class representation of an intercepted packet from WinDivert. This could be a variety
// of different types packets (e.g. TCP or UDP)
//

class Packet {
public:
	unsigned char packet[MAXBUF];
	UINT packet_len;
	WINDIVERT_ADDRESS addr;
	PWINDIVERT_IPHDR ip_header;
	PWINDIVERT_IPV6HDR ipv6_header;
	PWINDIVERT_ICMPHDR icmp_header;
	PWINDIVERT_ICMPV6HDR icmpv6_header;
	PWINDIVERT_TCPHDR tcp_header;
	PWINDIVERT_UDPHDR udp_header;
	PVOID payload;
	UINT payload_len;
};

//
// Structure repsenting a TCP/IP packet
//
typedef struct
{
	WINDIVERT_IPHDR  ip;
	WINDIVERT_TCPHDR tcp;
	UINT8 data[];
} TCP_PACKET, * PTCP_PACKET;