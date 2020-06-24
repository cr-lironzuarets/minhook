// FileReader.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <Windows.h>

typedef PVOID(*READTHEFILE)();


#define BUFFERSIZE 10
DWORD g_BytesTransferred = 0;


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

EXTERN_C __declspec(dllexport) PVOID ReadMyFile()
{
    HANDLE hFile;
    DWORD  dwBytesRead = 0;
    char*   ReadBuffer = (char*)malloc(BUFFERSIZE);
    memset(ReadBuffer, 0, BUFFERSIZE);
    OVERLAPPED ol = { 0 };

    hFile = CreateFile(
        L"example1.txt",               // file to open
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

int main()
{
    std::cout << "Hello World!\n";

    HMODULE hmod = LoadLibrary(L"FileReaderLib.dll");
    READTHEFILE proc = (READTHEFILE)GetProcAddress(hmod, "ReadTheFile");

    printf("example1.txt: %s\n", ReadMyFile());

    if (proc != NULL)
    {
        printf("example2.txt: %s", proc());
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started:
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
