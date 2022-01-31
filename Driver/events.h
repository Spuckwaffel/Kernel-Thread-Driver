#pragma once


bool Connect() {

	process::base_address = (ULONG64)PsGetProcessSectionBaseAddress(process::process);
	//print("\n[+] base address:    0x%llX", process::base_address);

	process::STRUCT_OFFSET_ADDRESS = ULONG64(process::base_address + OUTPUT_ADDRESS);
	//print("\n[+] struct address:  0x%llX -> offset: 0x%llX", process::STRUCT_OFFSET_ADDRESS, (ULONG64)(process::STRUCT_OFFSET_ADDRESS - process::base_address)); //address where the struct address get stored

	process::STATUS_CODE_ADDRESS = ULONG64(process::base_address + CODE_ADDRESS);
	process::STATUS_CODE_ADDRESS_REAL = readlocal<ULONG64>(process::STATUS_CODE_ADDRESS); //get the real address
	//print("\n[+] code address:    0x%llX -> offset: 0x%llX => 0x%llX", process::STATUS_CODE_ADDRESS, (ULONG64)(process::STATUS_CODE_ADDRESS - process::base_address), process::STATUS_CODE_ADDRESS_REAL); //this just saves the address to the status 
	

	int CURRCODE = readlocal<int>(process::STATUS_CODE_ADDRESS_REAL); // read it
	//print("\n[+] code: %d", CURRCODE);
	
	if (CURRCODE != 3) {
		return false;
	}
	//print("\n[+] connected!");
	status::ONLINE();

	return true;
}

void Disconnect() {
	//print("\n[+] stopping thread and closing...");
	

	PVOID Kthread = reinterpret_cast<PVOID>(KeGetCurrentThread());
	PVOID InitialStack		= (PVOID)((ULONG64)Kthread + GInitialStack);
	PVOID VCreateTime		= (PVOID)((ULONG64)Kthread + GVCreateTime);
	PVOID StartAddress		= (PVOID)((ULONG64)Kthread + GStartAddress);
	PVOID Win32StartAddress	= (PVOID)((ULONG64)Kthread + GWin32StartAddress);
	PVOID KernelStack		= (PVOID)((ULONG64)Kthread + GKernelStack);
	PVOID ExitStatus		= (PVOID)((ULONG64)Kthread + GExitStatus);
	PVOID CID				= (PVOID)((ULONG64)Kthread + GCID);


	//print("\n[+] resetting vars....");
	*(PVOID*)(VCreateTime)			= _VCreateTime;
	*(PVOID*)(StartAddress)			= _StartAddress;
	*(PVOID*)(Win32StartAddress)	= _Win32StartAddress;
	*(PVOID*)(KernelStack)			= _KernelStack;
	*(PVOID*)(ExitStatus)			= _ExitStatus;
	*(PVOID*)(CID)					= _CID;

	//print("\n[+] Bye!");
	PsTerminateSystemThread(STATUS_SUCCESS);
}

void InitTarget() {
	process::target_pid = readlocal<ULONG64>(readlocal<ULONG64>(process::STRUCT_OFFSET_ADDRESS));//double reading because STRUCT_OFFSET_ADDRESS saves a pointer that has pid
	//print("\n[+] process::target_pid: %d", process::target_pid);
	if (process::target_pid != 0) {
		status::SUCESSFUL();
	}
	else {
		status::ERROR();
	}
}

void GetBase() {
	if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)process::target_pid, &process::target_process))) {
		ULONG64 base = (ULONG64)PsGetProcessSectionBaseAddress(process::target_process);
		ULONG64 StructAddress = readlocal<ULONG64>(process::STRUCT_OFFSET_ADDRESS); //no double because direct writing
		writelocal<ULONG64>(&base, (PVOID)StructAddress);
		status::SUCESSFUL();
	}
	else {
		status::ERROR();
	}
}

void Read() {
	readd StructAddress = {};
	SIZE_T BytesRead{ 0 };
	(MmCopyVirtualMemory(process::process, (PVOID)readlocal<ULONG64>(process::STRUCT_OFFSET_ADDRESS), PsGetCurrentProcess(), &StructAddress, sizeof(readd), KernelMode, &BytesRead));

	//StructAddress = readlocal<readd>(readlocal<ULONG64>(process::STRUCT_OFFSET_ADDRESS)); //double reading because STRUCT_OFFSET_ADDRESS points to the struct and then we read the content
	if (StructAddress.address < 0x7FFFFFFFFFFF && StructAddress.address > 0 && StructAddress.size > 0 && StructAddress.size < 200) {
		read(StructAddress.address, StructAddress.output, StructAddress.size);
	}
	
	status::SUCESSFUL(); //we cant check if its unsuccessful
}