#include "defines.h"




void Check() {
	while (!NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)process::pid, &process::process))) {
		ObDereferenceObject(process::process);
		sleep(995);
	}
	//process::pid = reinterpret_cast<ULONG>(PsGetProcessId(process::process));
	print("\n[+] found process! Pid: %i", process::pid);

}

template< typename T >
void SpoofAddress(PVOID address, PVOID* save, PVOID target = 0) {
	*save = *(PVOID*)(address); //save current stuff
	//print("\n[+] saved orig data");
	*(PVOID*)(address) = target; //overwrite
	T spoof = *(T*)((ULONG64)address);
	//print("\n[+] spoofed! -> 0x%llX (%d)", spoof, spoof);
}

void HideThread() {

	PVOID ntoskrnlbase = (PVOID)((ULONG64)get_system_module_base(skCrypt("\\SystemRoot\\system32\\ntoskrnl.exe")) + 0x23810a);
	//PVOID ntoskrnlbase = (PVOID)((ULONG64)get_system_module_base("\\SystemRoot\\system32\\ntoskrnl.exe"));
	//print("\n[+] Base: 0x%llX", ntoskrnlbase);
	PVOID Kthread = reinterpret_cast<PVOID>(KeGetCurrentThread());
	//print("\n[+] Kthread: 0x%llX", Kthread);

	PVOID InitialStack		= (PVOID)((ULONG64)Kthread + GInitialStack);
	PVOID VCreateTime		= (PVOID)((ULONG64)Kthread + GVCreateTime);
	PVOID StartAddress		= (PVOID)((ULONG64)Kthread + GStartAddress);
	PVOID Win32StartAddress = (PVOID)((ULONG64)Kthread + GWin32StartAddress);
	PVOID KernelStack		= (PVOID)((ULONG64)Kthread + GKernelStack);
	PVOID CID				= (PVOID)((ULONG64)Kthread + GCID);
	PVOID ExitStatus		= (PVOID)((ULONG64)Kthread + GExitStatus);
	
	//print("\n[+] CreateTime: 0x%llX", VCreateTime);
	SpoofAddress<LARGE_INTEGER>(VCreateTime, &_VCreateTime, (PVOID)2147483247);

	//print("\n[+] StartAddress: 0x%llX", StartAddress);
	SpoofAddress<void*>(StartAddress, &_StartAddress, ntoskrnlbase);

	//print("\n[+] Win32StartAddress: 0x%llX", Win32StartAddress);
	SpoofAddress<void*>(Win32StartAddress, &_Win32StartAddress, ntoskrnlbase);

	//print("\n[+] KernelStack: 0x%llX", KernelStack);
	SpoofAddress<LARGE_INTEGER>(KernelStack, &_KernelStack);

	SpoofAddress<CLIENT_ID>(CID, &_CID);


	SpoofAddress<LONG>(ExitStatus, &_ExitStatus);

}

int errors = 0;
int CheckCode() {

	return readlocal<int>(process::STATUS_CODE_ADDRESS_REAL);
}
OSVERSIONINFOW GetOSVersion() {
	OSVERSIONINFOW OSInfo{ 0 };
	RtlGetVersion(&OSInfo);
	return OSInfo;
}

void mainthread()
{
	//KeSetBasePriorityThread(KeGetCurrentThread(), 31);

	auto OsInfo = GetOSVersion();

	if (OsInfo.dwBuildNumber > 19000) { //above 1909
		GInitialStack		= InitialStack_UP;
		GVCreateTime		= VCreateTime_UP;
		GStartAddress		= StartAddress_UP;
		GWin32StartAddress	= Win32StartAddress_UP;
		GImageFileName		= ImageFileName_UP;
		GActiveThreads		= ActiveThreads_UP;
		GActiveProcessLinks = ActiveProcessLinks_UP;
		GKernelStack		= KernelStack_UP;
		GExitStatus			= ExitStatus_UP;
		GCID				= CID_UP;
	}
	else {
		GInitialStack		= InitialStack_1909;
		GVCreateTime		= VCreateTime_1909;
		GStartAddress		= StartAddress_1909;
		GWin32StartAddress	= Win32StartAddress_1909;
		GImageFileName		= ImageFileName_1909;
		GActiveThreads		= ActiveThreads_1909;
		GActiveProcessLinks = ActiveProcessLinks_1909;
		GKernelStack		= KernelStack_1909;
		GExitStatus			= ExitStatus_1909;
		GCID				= CID_1909;
	}

	HideThread();
	//print("\n[+] waiting for program");

	sleep(1000);
	Check();
	sleep(3000);
	if (Connect()) {
		while (true) {
			int code = CheckCode();
			switch (code) {
			case 3:
				Disconnect();
				break;
			case 4: 
				Read();
				break;
			case 5:
				GetBase();
				break;
			case 6:
				InitTarget();
				break;
			default:
				break;
			}
		}
	}
	else {
		//print("\n[+] Failed to connect! Disconnecting....");
		Disconnect();
	}


}


NTSTATUS EntryPoint(ULONG64 mdl, ULONG64 code, ULONG64 output, ULONG64 PID)
{
	OUTPUT_ADDRESS = output;
	CODE_ADDRESS = code;

	MDL* mdlptr = reinterpret_cast<MDL*>(mdl);
	if (!null_pfn(mdlptr)) {
		return STATUS_UNSUCCESSFUL;
	}
	HANDLE thread_handle = nullptr;
	print("[+] PID: %d", PID);
	print("[+] code: %d", code);
	print("[+] output: %d", output);
	process::pid = PID;

	OBJECT_ATTRIBUTES object_attribues{ };
	InitializeObjectAttributes(&object_attribues, nullptr, OBJ_KERNEL_HANDLE, nullptr, nullptr);

	NTSTATUS status = PsCreateSystemThread(&thread_handle, 0, &object_attribues, nullptr, nullptr, reinterpret_cast<PKSTART_ROUTINE>(&mainthread), nullptr);
	//ZwClose(thread_handle);
	//print("\n[+] Bye bye DriverEntry!");
	return STATUS_UNSUCCESSFUL;
}

//NTSTATUS DriverEntry(PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path) {
//
//	UNREFERENCED_PARAMETER(driver_object);
//	UNREFERENCED_PARAMETER(registry_path);
//
//	//print("\n[+] Started driver!");
//
//	HANDLE thread_handle = nullptr;
//	OBJECT_ATTRIBUTES object_attribues{ };
//	InitializeObjectAttributes(&object_attribues, nullptr, OBJ_KERNEL_HANDLE, nullptr, nullptr);
//
//	NTSTATUS status = PsCreateSystemThread(&thread_handle, 0, &object_attribues, nullptr, nullptr, reinterpret_cast<PKSTART_ROUTINE>(&mainthread), nullptr);
//	//print("\n[+] Bye bye DriverEntry!");
//	return STATUS_SUCCESS;
//
//}
