# Overview
PortBender is a TCP port redirection utility that allows a red team operator to redirect inbound traffic destined for one TCP port (e.g., 445/TCP) to another TCP port (e.g., 8445/TCP). PortBender includes an aggressor script that operators can leverage to integrate the tool with Cobalt Strike. However, because the tool is implemented as a reflective DLL, it can integrate with any C2 framework supporting loading modules through a "ReflectiveLoader" interface [1]. The tool also allows operators to simulate a backdoor/persistence mechanism leveraged within the "PortServ.sys" capability used by the Duqu 2.0 threat actor. 

# Design
PortBender leverages the WinDivert library to intercept network traffic using the Windows Filtering Platform (WFP). The design of PortBender is heavily influenced by the DivertTCPConn utility which also leverages the WinDivert library [1]. 

# Usage
PortBender has two modes of operation. The first is "redirector mode," and the second is "backdoor mode." In "redirector mode," any connection to a targeted destination port (e.g., 445/TCP) is redirected to an alternative port (e.g., 8445/TCP). In "backdoor mode," we only redirect traffic if an attacker sends a specially formatted TCP packet to a target port (e.g., 443/TCP). PortBender then adds that client IP address to a list of backdoor clients and redirects all traffic to that target port to an alternative port (e.g., 3389/TCP). An operator can leverage this mechanism to emulate the persistence technique used by the Duqu 2.0 threat actor when compromising Kaspersky.

To execute PortBender we must first import the "PortBender.cna" script into Cobalt Strike and upload the WinDivert32.sys or WinDivert64.sys binary included in "PortBender.zip" to the target host depending on the operating system architecture. The help menu for PortBender with the example usage is shown below:

```
beacon> help PortBender
Redirect Usage: PortBender redirect FakeDstPort RedirectedPort
Backdoor Usage: PortBender backdoor FakeDstPort RedirectedPort Password
Examples:
	PortBender redirect 445 8445
	PortBender backdoor 443 3389 praetorian.antihacker
```
# Example Usage
For example, we may wish to execute PortBender in redirector mode to perform an SMB relay attack from a compromised Windows system.  To facilitate this, we can instruct PortBender to redirect all traffic to 445/TCP to an alternative port 8445/TCP running an attacker SMB service. In this example, we run the command "PortBender redirect 445 8445" to accomplish this. The expected output is below:

<p align="center">
<img width="811" alt="" src="https://user-images.githubusercontent.com/45573557/120108819-63adfa00-c12c-11eb-9c0d-73ae40f7320a.png">
</p>

In this example, we want to deploy the covert persistence mechanism on a compromised Internet-facing IIS webserver. Here we run the "PortBender backdoor 443 3389 praetorian.antihacker" to instruct the backdoor service to redirect any connections to 443/TCP to 3389/TCP on the compromised host from any IP address that provides the specified "praetorian.antihacker" keyword. The expected output is shown below:

<p align="center">
<img width="808" alt="" src="https://user-images.githubusercontent.com/45573557/120116246-e6df4800-c14c-11eb-93e0-a90640dfa02d.png">
</p>

# Acknowledgements

* Arno0x0x for his work on DivertTCPConn [1]
* Stephen Fewer for his work on Reflective DLL Injection [2]
* Basil00 for his work on WinDivert [3]
* Francisco Dominguez for his research into performing SMB relaying on Windows [4] 

# References
[1] https://github.com/Arno0x/DivertTCPconn \
[2] https://github.com/stephenfewer/ReflectiveDLLInjection \
[3] https://github.com/basil00/Divert \
[4] https://diablohorn.com/2018/08/25/remote-ntlm-relaying-through-meterpreter-on-windows-port-445
