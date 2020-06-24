// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <memory>

#define BUFFERSIZE 10
DWORD g_BytesTransferred = 0;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

VOID CALLBACK FileIOCompletionRoutine(
    __in  DWORD dwErrorCode,
    __in  DWORD dwNumberOfBytesTransfered,
    __in  LPOVERLAPPED lpOverlapped
);

VOID CALLBACK FileIOCompletionRoutine(
    __in  DWORD dwErrorCode,
    __in  DWORD dwNumberOfBytesTransfered,
    __in  LPOVERLAPPED lpOverlapped)
{
    g_BytesTransferred = dwNumberOfBytesTransfered;
}


EXTERN_C __declspec(dllexport) PVOID ReadTheFile()
{
    HANDLE hFile;
    DWORD  dwBytesRead = 0;
    char*   ReadBuffer = (char*)malloc(BUFFERSIZE);
    memset(ReadBuffer, 0, BUFFERSIZE);
    OVERLAPPED ol = { 0 };

    hFile = CreateFile(
        L"example2.txt",               // file to open
        GENERIC_READ,          // open for reading
        FILE_SHARE_READ,       // share for reading
        NULL,                  // default security
        OPEN_EXISTING,         // existing file only
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // normal file
        NULL);                 // no attr. template

    if (hFile == INVALID_HANDLE_VALUE)
    {
        return NULL;
    }

    if (FALSE == ReadFileEx(hFile, ReadBuffer, BUFFERSIZE - 1, &ol, FileIOCompletionRoutine))
    {
        CloseHandle(hFile);
        return NULL;
    }
    SleepEx(5000, TRUE);
    dwBytesRead = g_BytesTransferred;

    if (dwBytesRead > 0 && dwBytesRead <= BUFFERSIZE - 1)
    {
        ReadBuffer[dwBytesRead] = '\0'; // NULL character

    }
    CloseHandle(hFile);

    return ReadBuffer;
}
