#pragma once

//up because theyre all the same (19000 - ...)


#define InitialStack_1909		0x28
#define KernelStack_1909		0x58
#define VCreateTime_1909		0x600
#define StartAddress_1909		0x620
#define CID_1909				0x648
#define Win32StartAddress_1909	0x6a0
#define ExitStatus_1909			0x710



#define InitialStack_UP			0x28
#define KernelStack_UP			0x58
#define VCreateTime_UP			0x430
#define StartAddress_UP			0x450
#define CID_UP					0x478
#define Win32StartAddress_UP	0x4d0
#define ExitStatus_UP			0x548

#define ImageFileName_1909		0x450
#define ActiveThreads_1909		0x498
#define ActiveProcessLinks_1909	0x2F0

#define ImageFileName_UP		0x5a8
#define ActiveThreads_UP		0x5f0
#define ActiveProcessLinks_UP	0x448

namespace process
{
	ULONG pid;
	ULONG target_pid;
	PEPROCESS process;
	PEPROCESS target_process;
	ULONG64 base_address;
	ULONG64 STATUS_CODE_ADDRESS;
	ULONG64 STRUCT_OFFSET_ADDRESS;
	ULONG64 STATUS_CODE_ADDRESS_REAL;
}

ULONG GInitialStack;
ULONG GVCreateTime;
ULONG GStartAddress;
ULONG GWin32StartAddress;
ULONG GKernelStack;
ULONG GImageFileName;
ULONG GActiveThreads;
ULONG GActiveProcessLinks;
ULONG GExitStatus;
ULONG GCID;

PVOID _VCreateTime;
PVOID _StartAddress;
PVOID _Win32StartAddress;
PVOID _KernelStack;
PVOID _ExitStatus;
PVOID _CID;

typedef struct readd {
	ULONG64 address;
	ULONGLONG size;
	ULONG64 output;
};

typedef enum _SYSTEM_INFORMATION_CLASS
{
	SystemBasicInformation,
	SystemProcessorInformation,
	SystemPerformanceInformation,
	SystemTimeOfDayInformation,
	SystemPathInformation,
	SystemProcessInformation,
	SystemCallCountInformation,
	SystemDeviceInformation,
	SystemProcessorPerformanceInformation,
	SystemFlagsInformation,
	SystemCallTimeInformation,
	SystemModuleInformation = 0x0B
} SYSTEM_INFORMATION_CLASS, * PSYSTEM_INFORMATION_CLASS;

typedef struct _RTL_PROCESS_MODULE_INFORMATION
{
	HANDLE Section;
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	UCHAR  FullPathName[256];
} RTL_PROCESS_MODULE_INFORMATION, * PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES
{
	ULONG NumberOfModules;
	RTL_PROCESS_MODULE_INFORMATION Modules[1];
} RTL_PROCESS_MODULES, * PRTL_PROCESS_MODULES;

extern "C"
{
	NTKERNELAPI
		PVOID
		PsGetProcessSectionBaseAddress(
			PEPROCESS Process
		);

	NTKERNELAPI
		PPEB
		NTAPI
		PsGetProcessPeb(
			PEPROCESS Process
		);

	NTSTATUS NTAPI MmCopyVirtualMemory(
		PEPROCESS SourceProcess,
		PVOID SourceAddress,
		PEPROCESS TargetProcess,
		PVOID TargetAddress,
		SIZE_T BufferSize,
		KPROCESSOR_MODE PreviousMode,
		PSIZE_T ReturnSize
	);

	NTSYSCALLAPI
		NTSTATUS
		NTAPI
		ZwQuerySystemInformation(
			ULONG InfoClass,
			PVOID Buffer,
			ULONG Length,
			PULONG ReturnLength
		);

	NTSYSCALLAPI
		NTSTATUS
		ZwQueryInformationProcess(
			HANDLE ProcessHandle,
			PROCESSINFOCLASS ProcessInformationClass,
			PVOID ProcessInformation,
			ULONG ProcessInformationLength,
			PULONG ReturnLength
		);

	NTSYSCALLAPI
		NTSTATUS
		NTAPI
		ZwProtectVirtualMemory(
			HANDLE ProcessHandle,
			PVOID* BaseAddress,
			PSIZE_T RegionSize,
			ULONG NewAccessProtection,
			PULONG OldAccessProtection
		);

	NTKERNELAPI
		PVOID
		NTAPI
		RtlFindExportedRoutineByName(
			_In_ PVOID ImageBase,
			_In_ PCCH RoutineName
		);
}