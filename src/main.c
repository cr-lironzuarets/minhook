#include "../include/MinHook.h"
#include "api.h"
#include <windows.h>
#include <winternl.h>

typedef NTSTATUS(NTAPI *NTREADFILE)(
    HANDLE,
    HANDLE,
    PIO_APC_ROUTINE,
    PVOID,
    PIO_STATUS_BLOCK,
    PVOID,
    ULONG,
    PLARGE_INTEGER,
    PULONG);

typedef NTSTATUS(NTAPI *NTALLOCATEVIRTUALMEMORY)(HANDLE, PVOID *, ULONG, PULONG, ULONG, ULONG);
NTSTATUS NTAPI NtAllocateVirtualMemory(HANDLE, PVOID *, ULONG, PULONG, ULONG, ULONG);

// Pointer for calling original fpReadFile.
NTREADFILE fpNtReadFile = NULL;
NTREADFILE fpNtReadFileOrig = NULL;


NTSTATUS NTAPI NtReadFile(
    HANDLE,
    HANDLE,
    PIO_APC_ROUTINE,
    PVOID,
    PIO_STATUS_BLOCK,
    PVOID,
    ULONG,
    PLARGE_INTEGER,
    PULONG);

NTSTATUS NTAPI MyNtReadFile(
    _In_     HANDLE           FileHandle,
    _In_opt_ HANDLE           Event,
    _In_opt_ PIO_APC_ROUTINE  ApcRoutine,
    _In_opt_ PVOID            ApcContext,
    _Out_    PIO_STATUS_BLOCK IoStatusBlock,
    _Out_    PVOID            Buffer,
    _In_     ULONG            Length,
    _In_opt_ PLARGE_INTEGER   ByteOffset,
    _In_opt_ PULONG           Key
)
{
    fpNtReadFileOrig(
        FileHandle,
        Event,
        ApcRoutine,
        ApcContext,
        IoStatusBlock,
        Buffer,
        Length,
        ByteOffset,
        Key
    );

    char str[] = "IDODI!";
    size_t size = sizeof(str);

    if (Length > size)
    {
        my_memcpy(Buffer, str, sizeof(str));
        Length = sizeof(str);
    }
}



BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved)
{
    HMODULE ntdll;
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        ntdll = MyGetModuleHandleW(L"ntdll.dll");
        fpNtReadFile = (NTREADFILE)(MyGetProcAddress(ntdll, "NtReadFile"));

        MySleep(10000);

        // Initialize MinHook.B
        if (MH_Initialize() != MH_OK)
        {
            return FALSE;
        }

        if (MH_CreateHook(fpNtReadFile, &MyNtReadFile,
            (LPVOID*)(&fpNtReadFileOrig)) != MH_OK)
        {
            return FALSE;
        }

        // Enable the hook.
        if (MH_EnableHook(fpNtReadFile) != MH_OK)
        {
            return FALSE;
        }
    }

    return TRUE;
}
