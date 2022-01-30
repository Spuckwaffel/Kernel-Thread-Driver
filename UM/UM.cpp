#include "stdafx.h"

DWORD pid;
DWORD64 baseaddress;

int main()
{
	//use your mapper code here:

	//send your driver following stuff: (use kdmapper or something else)

	//					  mdl pointer, STATUS_CODE_ADDRESS, STRUCT_OFFSET_ADDRESS,  pid
	//NTSTATUS EntryPoint(ULONG64 mdl, ULONG64 code,		ULONG64 output,			ULONG64 PID)


	std::cout << "Hello world!" << std::endl;
	Connect();

	pid = GetProcessId(L"FortniteClient-Win64-Shipping.exe");

	std::cout << "PID:" << pid << std::endl;
	//init our target
	if (!initTarget(pid)) return 1;

	baseaddress = GetBase();

	std::cout << "base: 0x" << std::hex << baseaddress << std::endl;

	//a test
	uint64_t UWORLD = Read<uint64_t>(0xB78BC70 + baseaddress);

	std::cout << "test: 0x" << std::hex << UWORLD << std::endl;

	Disconnect();

}