#include "pycontroller.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

#define BUFLEN 4096
#define CMD_EXEC 1
#define CMD_GETVAR 2

#define ERR_NOTRUNNING -1
#define ERR_BADRESULTLINE -2
#define ERR_BADRESULTCODE -3

#ifdef WIN32
// TODO
#else

#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

PyController::PyController()
{
	m_pythonExecutable = "python";
	m_scriptPath = "/home/jori/projects/rPython2/inst/pythonwrapperscript.py"; // TODO: what's a reasonable default
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
		setErrorString("Internal error: bad result line");
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

			read(m_resultPipe[0], &(buffer[0]), resultLength);
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
		setErrorString("Internal error: bad result line");
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
		if (read(m_resultPipe[0], &(variableBuffer[0]), resultLength) != resultLength)
		{
			setErrorString("Short read");
			return false;
		}
	}

	if (resultCode != 0)
	{
		setErrorString((char *)&(variableBuffer[0]));
		return false;
	}

	return true;
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

		char *argv[] = { pExec, pScript, resultDesc, 0 };
	
		cerr << "Starting python process: " << pExec << endl;
		if (execvp(pExec, argv) < 0) // environ is a global variable
		{
			cerr << "Unable to start python process" << endl;
			exit(-1); // stop child process
		}
		exit(0); // shouldn't get here (process was replaced by python), but just in case...
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

	if (line != "RPYTHON2")
	{
		cleanup();
		cerr << "Received bad identifier from python process" << endl;
		return false;
	}

	// Ok, we're up and running!

	//cerr << "Ok, started" << endl;

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
		if (read(m_resultPipe[0], c, 1) != 1)
		{
			setErrorString("Read error");
			cleanup();
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

#endif // WIN32

