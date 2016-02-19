#include "pycontroller.h"
#include <R.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>

#ifdef _WIN32
#include <strsafe.h>
#include <io.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#endif // _WIN32

using namespace std;

#define BUFLEN 4096
#define CMD_EXEC 1
#define CMD_GETVAR 2
#define SCRIPTIDENTIFIER "RPITHON"

std::string PyController::s_defaultPythonExecutable = "python";

void PyController::startupMessage()
{
	string ident = "(default)";
	if (m_identifier.length() > 0)
		ident = m_identifier;

	Rprintf("Starting python process: %s, identifier: %s\n", m_pythonExecutable.c_str(), ident.c_str());
	R_FlushConsole();
}

#ifdef _WIN32
PyController::PyController(const string &identifier) : m_identifier(identifier)
{
	m_pythonExecutable = s_defaultPythonExecutable;
	m_scriptPath = "pythonwrapperscript.py";
	m_hStdinPipe[0] = 0;
	m_hStdinPipe[1] = 0;
	m_hResultPipe[0] = 0;
	m_hResultPipe[1] = 0;
	m_hPyProcess= 0;
	m_hPyThread = 0;
	m_startTried = false;
}

PyController::~PyController()
{
	cleanup();
}

void PyController::cleanup()
{
	if (m_hStdinPipe[0])
		CloseHandle(m_hStdinPipe[0]);
	if (m_hStdinPipe[1])
		CloseHandle(m_hStdinPipe[1]);
	if (m_hResultPipe[0])
		CloseHandle(m_hResultPipe[0]);
	if (m_hResultPipe[1])
		CloseHandle(m_hResultPipe[1]);
	if (m_hPyProcess)
	{
		TerminateProcess(m_hPyProcess, -1);
		CloseHandle(m_hPyProcess);
	}
	if (m_hPyThread)
		CloseHandle(m_hPyThread);

	m_hStdinPipe[0] = 0;
	m_hStdinPipe[1] = 0;
	m_hResultPipe[0] = 0;
	m_hResultPipe[1] = 0;
	m_hPyProcess= 0;
	m_hPyThread = 0;
}

bool PyController::checkRunning()
{
	if (m_hPyProcess)
		return true;
	
	if (m_startTried)
	{
		setErrorString("Already tried to start the python process and failed");
		return false;
	}

	m_startTried = true;

	SECURITY_ATTRIBUTES secAtt;

	memset(&secAtt, 0, sizeof(SECURITY_ATTRIBUTES));
	secAtt.nLength = sizeof(SECURITY_ATTRIBUTES);
	secAtt.bInheritHandle = TRUE;

	if (!CreatePipe(&(m_hStdinPipe[0]), &(m_hStdinPipe[1]), &secAtt, 0))
	{
		setErrorString("Couldn't create input channel");
		return false;
	}
	if (!CreatePipe(&(m_hResultPipe[0]), &(m_hResultPipe[1]), &secAtt, 0))
	{
		setErrorString("Couldn't create result channel");
		return false;
	}
	if (!SetHandleInformation(m_hStdinPipe[1], HANDLE_FLAG_INHERIT, 0))
	{
		setErrorString("Couldn't set input channel inheritance flag");
		return false;
	}

	if (!SetHandleInformation(m_hResultPipe[0], HANDLE_FLAG_INHERIT, 0))
	{
		setErrorString("Couldn't set result channel inheritance flag");
		return false;
	}

	PROCESS_INFORMATION procInf;
	STARTUPINFO startInfo;

	memset(&procInf, 0, sizeof(PROCESS_INFORMATION));
	memset(&startInfo, 0, sizeof(STARTUPINFO));

	startInfo.cb = sizeof(STARTUPINFO);
	startInfo.dwFlags |= STARTF_USESTDHANDLES;
	//startInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	//startInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	startInfo.hStdOutput = (HANDLE)_get_osfhandle(fileno(stdout));
	startInfo.hStdError = (HANDLE)_get_osfhandle(fileno(stderr));
	startInfo.hStdInput = m_hStdinPipe[0];

	char execStr[BUFLEN];
	// -u is for unbuffered output, so you don't need to flush stdout to get
	// the result of a print command
	StringCbPrintf(execStr, BUFLEN, "\"%s\" -u \"%s\" 0x%p", m_pythonExecutable.c_str(), m_scriptPath.c_str(),
			                                      m_hResultPipe[1]);
	execStr[BUFLEN-1] = 0;

	startupMessage();

	if (!CreateProcess(0,
			   execStr,
		   	   0,0,TRUE, CREATE_NO_WINDOW, 0, 0, &startInfo, &procInf))
	{
		setErrorString("Couldn't start process");
     		return false;		
	}

	m_hPyProcess = procInf.hProcess;
	m_hPyThread = procInf.hThread;

	//cout << "Process handle is " << m_hPyProcess << endl; 
	//cout << "Thread handle is " << m_hPyThread << endl;

	CloseHandle(m_hStdinPipe[0]);
	m_hStdinPipe[0] = 0;

	CloseHandle(m_hResultPipe[1]);
	m_hResultPipe[1] = 0;

	string line;
	readLine(line);

	//cerr << "Read line: " << line << endl;

	if (line != SCRIPTIDENTIFIER)
	{
		cleanup();
		setErrorString("Received bad identifier from python process");
		return false;
	}

	// Ok, we're up and running!

	//cerr << "Ok, started" << endl;

	return true;
}

void PyController::writeCommand(int cmd, const void *pData, int dataLen)
{
	char str[BUFLEN];

	StringCbPrintf(str, BUFLEN, "%d,%d\n", cmd, dataLen);
	str[BUFLEN-1] = 0;

	//cout << "Writing command " << str << endl;
	
	DWORD num;
	WriteFile(m_hStdinPipe[1], str, strlen(str), &num, 0);
	//cout << "Writing data of length " << dataLen << endl;
	WriteFile(m_hStdinPipe[1], pData, dataLen, &num, 0);
	//cout << "Done" << endl;
}

#else

PyController::PyController(const string &identifier) : m_identifier(identifier)
{
	m_pythonExecutable = s_defaultPythonExecutable;
	m_scriptPath = "pythonwrapperscript.py";
	m_stdinPipe[0] = -1;
	m_stdinPipe[1] = -1;
	m_resultPipe[0] = -1;
	m_resultPipe[1] = -1;
	m_pythonPID = -1;
	m_startTried = false;
}

PyController::~PyController()
{
	cleanup();
}

void PyController::cleanup()
{
	if (m_stdinPipe[0] >= 0)
		close(m_stdinPipe[0]);
	if (m_stdinPipe[1] >= 0)
		close(m_stdinPipe[1]);
	if (m_resultPipe[0] >= 0)
		close(m_resultPipe[0]);
	if (m_resultPipe[1] >= 0)
		close(m_resultPipe[1]);
	if (m_pythonPID >= 0)
		kill(m_pythonPID, SIGKILL);

	m_stdinPipe[0] = -1;
	m_stdinPipe[1] = -1;
	m_resultPipe[0] = -1;
	m_resultPipe[1] = -1;
	m_pythonPID = -1;
}

bool PyController::checkRunning()
{
	if (m_pythonPID > 0)
		return true;
	
	if (m_startTried)
	{
		setErrorString("Already tried to start the python process and failed");
		return false;
	}

	m_startTried = true;

	pipe(m_stdinPipe);
	pipe(m_resultPipe);

	m_pythonPID = fork();
	if (m_pythonPID < 0)
	{
		cleanup();
		setErrorString("Unable to fork child process");
		return false;
	}

	//cerr << "Fork successful" << endl;

	startupMessage();

	if (m_pythonPID == 0) // In this case, we're in the child process
	{
		// close default stdin and replace with our pipe
		close(0); 
		dup(m_stdinPipe[0]);

		close(m_stdinPipe[1]);
		close(m_resultPipe[0]);

		char *pExec = (char *)m_pythonExecutable.c_str();
		char *pScript = (char *)m_scriptPath.c_str();
		char resultDesc[256];

		snprintf(resultDesc, 256, "%d", m_resultPipe[1]);
		resultDesc[255] = 0;

		// -u is for unbuffered output, so you don't need to flush stdout to get
		// the result of a print command
		char *argv[] = { pExec, "-u", pScript, resultDesc, 0 };

		if (execvp(pExec, argv) < 0) // environ is a global variable
		{
			//cerr << "Unable to start python process" << endl;
			_exit(-1); // stop child process
		}
		_exit(0); // shouldn't get here (process was replaced by python), but just in case...
	}

	close(m_stdinPipe[0]);
	m_stdinPipe[0] = -1;

	close(m_resultPipe[1]);
	m_resultPipe[1] = -1;

	//cerr << "Child process has PID " << pythonPID << endl;

	// We're the parent process, we can send commands using stdinPipe[1] and read results using resultPipe[0]
	string line;

	readLine(line);

	//cerr << "Read line: " << line << endl;

	if (line != SCRIPTIDENTIFIER)
	{
		cleanup();
		setErrorString("Received bad identifier from python process (perhaps python program couldn't be started?)");
		return false;
	}

	// Ok, we're up and running!

	//cerr << "Ok, started" << endl;

	return true;
}

void PyController::writeCommand(int cmd, const void *pData, int dataLen)
{
	char str[BUFLEN];

	snprintf(str, BUFLEN, "%d,%d\n", cmd, dataLen);
	//cout << "Writing command " << str << endl;
	write(m_stdinPipe[1], str, strlen(str));
	//cout << "Writing data of length " << dataLen << endl;
	write(m_stdinPipe[1], pData, dataLen);
	//cout << "Done" << endl;
}

#endif // _WIN32

bool PyController::exec(const std::string &code)
{
	//cout << "code: " << code << endl;

	if (!checkRunning())
		return false;

	writeCommand(CMD_EXEC, code.c_str(), code.length());

	string line;

	if (!readLine(line))
		return false;

	int resultCode, resultLength;

	if (sscanf(line.c_str(), "%d,%d", &resultCode, &resultLength) != 2)
	{
		setErrorString("Internal error: bad result line: " + line);
		return false;
	}

	// Here, a correct response is a result code of 0, with no extra length
	if (resultCode < 0 || (resultCode == 0 && resultLength != 0))
	{
		setErrorString("Internal error: bad result code");
		return false;
	}

	if (resultCode != 0)
	{
		if (resultLength > 0)
		{
			// Result should be an error description from the python process
			vector<uint8_t> buffer(resultLength+1);

#ifdef _WIN32
			DWORD num = 0;
			ReadFile(m_hResultPipe[0], &(buffer[0]), resultLength, &num,0);
#else
			read(m_resultPipe[0], &(buffer[0]), resultLength);
#endif
			buffer[resultLength] = 0;

			setErrorString((char *)&(buffer[0]));
		}
		else
		{
			char str[BUFLEN];

			snprintf(str, BUFLEN, "Error code %d", resultCode);
			str[BUFLEN-1] = 0;

			setErrorString(str);
		}
		return false;
	}

	return true;
}

bool PyController::getVariable(const std::string &name, std::vector<uint8_t> &variableBuffer)
{
	if (!checkRunning())
		return false;

	writeCommand(CMD_GETVAR, name.c_str(), name.length());

	string line;

	if (!readLine(line))
		return false;

	int resultCode, resultLength;

	if (sscanf(line.c_str(), "%d,%d", &resultCode, &resultLength) != 2)
	{
		setErrorString("Internal error: bad result line: " + line);
		return false;
	}

	// Here, a correct response is a result code of 0 (or positive if error in
	// the python script), with the variable contents that follows
	if (resultCode < 0)
	{
		setErrorString("Internal error: bad result code");
		return false;
	}

	variableBuffer.resize(resultLength+1);
	variableBuffer[resultLength] = 0;

	if (resultLength != 0)
	{
#ifdef _WIN32
		DWORD num = 0;
		BOOL success = ReadFile(m_hResultPipe[0], &(variableBuffer[0]), resultLength, &num, 0);
		if (!success || num != resultLength)
		{
			setErrorString("Short read");
			return false;
		}
#else
		int bytesToRead = resultLength;
		int bytesRead = 0;
		while (bytesToRead > 0)
		{
			int status;
			status = read(m_resultPipe[0], &(variableBuffer[bytesRead]), bytesToRead);
			if (status < 0)
			{
				char msg[1024];
				sprintf(msg, "Got error %d while reading bytes (read %d, expected %d bytes)", status, bytesRead, resultLength);
				setErrorString("Read error: " + string(msg));
				return false;
			}
			//if (status != bytesToRead)
			//	cout << "Didn't read everything at once!" << endl;
			bytesRead += status;
			bytesToRead -= status;
		}
#endif // _WIN32
	}

	if (resultCode != 0)
	{
		setErrorString((char *)&(variableBuffer[0]));
		return false;
	}

	return true;
}

bool PyController::readLine(string &line)
{
	//cout << "Reading a line" << endl;

	string result = "";

	// This is not efficient, but these lines should be short anyway
	bool done = false;
	while (!done)
	{
		char c[2];

		c[1] = 0;
		//cout << "Reading a character..." << endl;
#ifdef _WIN32
		DWORD numRead = 0;
		BOOL success = ReadFile(m_hResultPipe[0], c, 1, &numRead, 0);
		if (!success || numRead != 1)
#else

		if (read(m_resultPipe[0], c, 1) != 1)
#endif // _WIN32
		{
			setErrorString("Read error");
			return false;
		}
		else
		{
			if (c[0] == '\n')
				done = true;
			else
				result += string(c);
		}
	}
	//cout << "Done." << endl;

	line = result;
	return true;
}


