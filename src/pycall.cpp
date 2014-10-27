#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>
#include <sys/select.h>
#include <sys/wait.h>

using namespace std;

static string pythonPath;
static bool pythonStarted = false;
static int stdinPipe[2] = { -1, -1 };
static int resultPipe[2] = { -1, -1 };

static int pythonPID = -1;
static bool startTried = false;

static string lastErrorMessage = "No error";
static vector<uint8_t> variableBuffer;

extern "C" 
{
	void py_init();
	void py_close();
	void py_exec_code(const char** code, int* exit_status);
	void py_get_var(const char** var_name, int* found, char** resultado);
}

void py_init()
{
	pythonPath = "/usr/bin/python";
}

void py_close()
{
	// TODO: close pipes, kill child process
}

#define BUFLEN 4096
#define CMD_EXEC 1
#define CMD_GETVAR 2

#define ERR_NOTRUNNING -1
#define ERR_BADRESULTLINE -2
#define ERR_BADRESULTCODE -3

string readLine(int fileDesc);

void closePids()
{
	close(resultPipe[0]);
	close(resultPipe[1]);
	close(stdinPipe[0]);
	close(stdinPipe[1]);

	resultPipe[0] = -1;
	resultPipe[1] = -1;
	stdinPipe[0] = -1;
	stdinPipe[1] = -1;
}

bool checkRunning()
{
	if (pythonPID > 0)
		return true;
	
	if (startTried)
		return false;

	startTried = true;

	pipe(stdinPipe);
	pipe(resultPipe);

	pythonPID = fork();
	if (pythonPID < 0)
	{
		cerr << "Unable to fork child process";
		return false;
	}

	cerr << "Fork successful" << endl;

	if (pythonPID == 0) // In this case, we're in the child process
	{
		// close default stdin and replace with our pipe
		close(0); 
		dup(stdinPipe[0]);

		//char *pExec = (char *)pythonPath.c_str();
		char pExec[] = "python";
//		char script[] = "pythonwrapperscript.py"; // TODO
		char script[] = "/home/jori/projects/rPython2/inst/pythonwrapperscript.py";
		char resultDesc[256];

		sprintf(resultDesc, "%d", resultPipe[1]);
		char * const argv[] = { pExec, script, resultDesc, 0 };
	
		cerr << "Starting python process" << endl;
		if (execvp(pExec, argv) < 0) // environ is a global variable
		{
			cerr << "Unable to start python process" << endl;
			exit(-1); // stop child process
		}
		exit(0); // shouldn't get here (process was replaced by python), but just in case...
	}

	cerr << "Child process has PID " << pythonPID << endl;

	// We're the parent process, we can send commands using stdinPipe[1] and read results using resultPipe[0]
	string line = readLine(resultPipe[0]);

	cerr << "Read line: " << line << endl;

	if (line != "RPYTHON2")
	{
		closePids();
		cerr << "Received bad identifier from python process" << endl;
		return false;
	}

	// Ok, we're up and running!

	cerr << "Ok, started" << endl;

	return true;
}

string readLine(int fileDesc)
{
	//cout << "Reading a line" << endl;

	string result = "";

	// This is not efficient, but these lines should be short anyway
	bool done = false;
	while (!done)
	{
		fd_set fdSet, exceptSet;
		struct timeval tv;

		FD_ZERO(&fdSet);
		FD_SET(fileDesc, &fdSet);
		FD_ZERO(&exceptSet);
		FD_SET(fileDesc, &exceptSet);

		tv.tv_sec = 1;
		tv.tv_usec = 0;

		//cout << "Starting select" << endl;
		if (waitpid(pythonPID, 0, WNOHANG) == pythonPID)
		{
			cout << "Python process finished" << endl;
			closePids();
			return "PYTHON FINISHED";
		}

		int status = select(FD_SETSIZE, &fdSet, 0, &exceptSet, &tv);
		if (status < 0)
		{
			cout << "Error in select" << endl;
			closePids();
			return "ERROR IN SELECT";
		}

		if (FD_ISSET(fileDesc, &exceptSet))
		{
			cout << "Select exception" << endl;
			closePids();
			return "SELECT EXCEPTION";
		}

		if (FD_ISSET(fileDesc, &fdSet)) // Ok, we can read something
		{
			char c[2];

			c[1] = 0;
			//cout << "Reading a character..." << endl;
			if (read(fileDesc, c, 1) != 1)
			{
				cout << "Read error" << endl;
				closePids();
				return "READ ERROR";
			}
			else
			{
				if (c[0] == '\n')
					done = true;
				else
					result += string(c);
			}
		}
	}
	//cout << "Done." << endl;

	return result;
}

void writeCommand(int cmd, const void *pData, int dataLen)
{
	char str[BUFLEN];

	snprintf(str, BUFLEN, "%d,%d\n", cmd, dataLen);
	//cout << "Writing command " << str << endl;
	write(stdinPipe[1], str, strlen(str));
	//cout << "Writing data of length " << dataLen << endl;
	write(stdinPipe[1], pData, dataLen);
	//cout << "Done" << endl;
}

void py_exec_code(const char** code, int* exit_status )
{
	if (!checkRunning())
	{
		*exit_status = ERR_NOTRUNNING;
		lastErrorMessage = "Python not running";
		return;
	}

	writeCommand(CMD_EXEC, *code, strlen(*code));

	string line = readLine(resultPipe[0]);
	int resultCode, resultLength;

	if (sscanf(line.c_str(), "%d,%d", &resultCode, &resultLength) != 2)
	{
		*exit_status = ERR_BADRESULTLINE;
		lastErrorMessage = "Internal error: bad result line";
		return;
	}

	// Here, a correct response is a result code of 0, with no extra length
	if (resultCode < 0 || (resultCode == 0 && resultLength != 0))
	{
		*exit_status = ERR_BADRESULTCODE;
		lastErrorMessage = "Internal error: bad result code";
		return;
	}

	if (resultLength != 0)
	{
		// Result should be an error description
		vector<uint8_t> buffer(resultLength+1);

		read(resultPipe[0], &(buffer[0]), resultLength);
		buffer[resultLength] = 0;

		lastErrorMessage = string((char *)&(buffer[0]));
	}

	*exit_status = resultCode;
}

void py_get_var( const char** var_name, int* found, char** resultado )
{
	if (!checkRunning())
	{
		*found = 1;
		lastErrorMessage = "Python not running";
		*resultado = (char *)lastErrorMessage.c_str();
		return;
	}

	*found = 0;

	writeCommand(CMD_GETVAR, *var_name, strlen(*var_name));

	string line = readLine(resultPipe[0]);
	int resultCode, resultLength;

	if (sscanf(line.c_str(), "%d,%d", &resultCode, &resultLength) != 2)
	{
		*found = 1;
		lastErrorMessage = "Internal error: bad result line";
		*resultado = (char *)lastErrorMessage.c_str();
		return;
	}

	// Here, a correct response is a result code of 0, with the variable contents that follows
	if (resultCode < 0)
	{
		*found = 1;
		lastErrorMessage = "Internal error: bad result code";
		*resultado = (char *)lastErrorMessage.c_str();
		return;
	}

	variableBuffer.resize(resultLength+1);
	variableBuffer[resultLength] = 0;

	if (resultLength != 0)
		read(resultPipe[0], &(variableBuffer[0]), resultLength);

	*resultado = (char *)&(variableBuffer[0]); // Is it ok to keep reusing this?
}
