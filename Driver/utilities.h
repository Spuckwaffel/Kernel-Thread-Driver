#pragma once


void sleep(int ms) { 
	
	LARGE_INTEGER time;  
	time.QuadPart = -(ms) * 10 * 1000; //10000 = 1ms 1000= 100ys 100 = 10ys 10= 1ys 
	KeDelayExecutionThread(KernelMode, TRUE, &time); 
}



NTSTATUS process_by_name(CHAR* process_name, PEPROCESS* process)
{
    PEPROCESS sys_process = PsInitialSystemProcess;
    PEPROCESS cur_entry = sys_process;
    CHAR image_name[15];

    do
    {
        RtlCopyMemory((PVOID)(&image_name), (PVOID)((uintptr_t)cur_entry + GImageFileName) /*EPROCESS->ImageFileName*/, sizeof(image_name));

        if (strstr(image_name, process_name))
        {
            ULONG active_threads;
            RtlCopyMemory((PVOID)&active_threads, (PVOID)((uintptr_t)cur_entry + GActiveThreads) /*EPROCESS->ActiveThreads*/, sizeof(active_threads));
            if (active_threads)
            {
                *process = cur_entry;
                return STATUS_SUCCESS;
            }
        }

        PLIST_ENTRY list = (PLIST_ENTRY)((uintptr_t)(cur_entry) + GActiveProcessLinks) /*EPROCESS->ActiveProcessLinks*/;
        cur_entry = (PEPROCESS)((uintptr_t)list->Flink - GActiveProcessLinks);

    } while (cur_entry != sys_process);

    return STATUS_NOT_FOUND;
}

PVOID get_system_module_base(const char* module_name)
{
    ULONG bytes = 0;
    NTSTATUS status = ZwQuerySystemInformation(SystemModuleInformation, NULL, bytes, &bytes);

    if (!bytes)
        return NULL;

    PRTL_PROCESS_MODULES modules = (PRTL_PROCESS_MODULES)ExAllocatePoolWithTag(NonPagedPool, bytes, 0x4e554c4c);

    status = ZwQuerySystemInformation(SystemModuleInformation, modules, bytes, &bytes);

    if (!NT_SUCCESS(status))
        return NULL;

    PRTL_PROCESS_MODULE_INFORMATION module = modules->Modules;
    PVOID module_base = 0, module_size = 0;

    for (ULONG i = 0; i < modules->NumberOfModules; i++) {
        if (strcmp((char*)module[i].FullPathName, module_name) == NULL) {
            module_base = module[i].ImageBase;
            module_size = (PVOID)module[i].ImageSize;
            break;
        }
    }

    if (0 == 0) {
        PEPROCESS process1;
    }

    if (modules)
        ExFreePoolWithTag(modules, NULL);

    if (module_base <= NULL)
        return NULL;

    return module_base;
}

PVOID get_system_module_export(const char* module_name, LPCSTR routine_name)
{
    PVOID lpModule = get_system_module_base(module_name);

    if (!lpModule)
        return NULL;

    return RtlFindExportedRoutineByName(lpModule, routine_name);
}

NTSTATUS  write_virtual_memory(ULONG pid, PEPROCESS process, PVOID source_address, PVOID target_address, SIZE_T size)
{
    SIZE_T bytes = 0;
    //print("writing to target: 0x%llX", target_address);
    if (NT_SUCCESS(MmCopyVirtualMemory(PsGetCurrentProcess(), source_address, process, target_address, size, KernelMode, &bytes)))
        return STATUS_SUCCESS;

    return STATUS_UNSUCCESSFUL;
}

NTSTATUS read_virtual_memory(ULONG pid, PEPROCESS process, PVOID source_address, PVOID target_address, SIZE_T size)
{
    SIZE_T bytes = 0;
    
    if (NT_SUCCESS(MmCopyVirtualMemory(process, source_address, PsGetCurrentProcess(), target_address, size, KernelMode, &bytes)))
        return STATUS_SUCCESS;

     return STATUS_SUCCESS;
}

template< typename T >
T readlocal(uintptr_t address)
{
    T buffer{};
    read_virtual_memory(process::pid, process::process, (void*)address, &buffer, sizeof(T));
    return buffer;
}

void read(uintptr_t from, uintptr_t to, ULONGLONG size)
{
    SIZE_T bytes = 0;
    if (size || to || from) MmCopyVirtualMemory(process::target_process, (PVOID)from, process::process, (PVOID)to, size, KernelMode, &bytes);
       
}

template< typename T >
void writelocal(PVOID from, PVOID to)
{
    write_virtual_memory(process::pid, process::process, from, to, sizeof(T));
}



namespace status {
    void SUCESSFUL() {
        int i = 1;
        write_virtual_memory(process::pid, process::process, &i, (PVOID)process::STATUS_CODE_ADDRESS_REAL, sizeof(i));
    }
    void ONLINE() { //idle
        int i = 0;
        write_virtual_memory(process::pid, process::process, &i, (PVOID)process::STATUS_CODE_ADDRESS_REAL, sizeof(i));
    }
    void ERROR() {
        int i = 2;
        write_virtual_memory(process::pid, process::process, &i, (PVOID)process::STATUS_CODE_ADDRESS_REAL, sizeof(i));
    }
}

bool null_pfn(PMDL mdl)
{
    PPFN_NUMBER mdl_pages = MmGetMdlPfnArray(mdl);
    if (!mdl_pages) { return false; }

    ULONG mdl_page_count = ADDRESS_AND_SIZE_TO_SPAN_PAGES(MmGetMdlVirtualAddress(mdl), MmGetMdlByteCount(mdl));
    //print("[+] mdl_page_count: %d", mdl_page_count);
    ULONG null_pfn = 0x0;
    MM_COPY_ADDRESS source_address = { 0 };
    source_address.VirtualAddress = &null_pfn;

    for (ULONG i = 0; i < mdl_page_count; i++)
    {
        size_t bytes = 0;
        MmCopyMemory(&mdl_pages[i], source_address, sizeof(ULONG), MM_COPY_MEMORY_VIRTUAL, &bytes);
    }
    return true;
}
