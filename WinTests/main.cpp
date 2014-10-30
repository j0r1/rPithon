#include <windows.h>
#include <strsafe.h>
#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <string>

void ErrorExit(PTSTR lpszFunction) 

// Format a readable error message, display a message box, 
// and exit from the application.
{ 
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf)+lstrlen((LPCTSTR)lpszFunction)+40)*sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(1);
}

using namespace std;

int main(void)
{
	HANDLE stdinPipe[2];
	HANDLE resultPipe[2];

	SECURITY_ATTRIBUTES secAtt;

	memset(&secAtt, 0, sizeof(SECURITY_ATTRIBUTES));
	secAtt.nLength = sizeof(SECURITY_ATTRIBUTES);
	secAtt.bInheritHandle = TRUE;

	if (!CreatePipe(&(stdinPipe[0]), &(stdinPipe[1]), &secAtt, 0))
		return -1;
	if (!CreatePipe(&(resultPipe[0]), &(resultPipe[1]), &secAtt, 0))
		return -1;

	if (!SetHandleInformation(stdinPipe[1], HANDLE_FLAG_INHERIT, 0))
		return -1;

	if (!SetHandleInformation(resultPipe[0], HANDLE_FLAG_INHERIT, 0))
		return -1;

	PROCESS_INFORMATION procInf;
	STARTUPINFO startInfo;

	memset(&procInf, 0, sizeof(PROCESS_INFORMATION));
	memset(&startInfo, 0, sizeof(STARTUPINFO));

	startInfo.cb = sizeof(STARTUPINFO);
	startInfo.dwFlags |= STARTF_USESTDHANDLES;
	startInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	startInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	startInfo.hStdInput = stdinPipe[0];
	
	char execStr[1024];
	StringCbPrintf(execStr, 1024, "python test.py 0x%p", resultPipe[1]);

	if (!CreateProcess(0,
			   execStr,
		   	   0,0,TRUE, 0, 0, 0, &startInfo, &procInf))
	{
		cerr << "Couldn't start process" << endl;
		ErrorExit("CreateProcess");
     		return -1;		
	}

	CloseHandle(stdinPipe[0]);
	CloseHandle(resultPipe[1]);

	string cmd = "print 'HeyHey'\n";
	DWORD bytesWritten = 0;
	if (!WriteFile(stdinPipe[1], cmd.c_str(), cmd.length(), &bytesWritten, 0))
	{
		cerr << "Couldn't write" << endl;
		return -1;
	}

	char buffer[1024];
	DWORD bytesRead = 0;

	if (!ReadFile(resultPipe[0], buffer, 10, &bytesRead, 0))
	{
		cerr << "Couldn't read" << endl;
		return -1;
	}
	
	buffer[bytesRead] = 0;
	cout << "Parent read: " << buffer << endl;

	return 0;
}

