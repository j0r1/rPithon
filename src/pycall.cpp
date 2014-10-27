#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <string>
#include <vector>

using namespace std;

static string pythonPath;
static bool pythonStarted = false;
static int stdinPipe[2] = { -1, -1 };
static int resultPipe[2] = { -1, -1 };
static int pythonPID = -1;

static string lastErrorMessage = "No error";
static vector<uint8_t> variableBuffer;

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

bool checkRunning()
{
	return false; // TODO
}

string readLine(int fileDesc)
{
	string result = 0;

	// This is not efficient, but these lines should be short anyway
	bool done = false;
	while (!done)
	{
		char c[1];

		if (read(fileDesc, c, 1) != 1)
		{
			done = true;
			result = "READ ERROR";
		}
		else
		{
			if (c[0] == '\n')
				done = true;
			else
				result += string(c);
		}
	}

	return result;
}

void writeCommand(int cmd, uint8_t *pData, int dataLen)
{
	char str[BUFLEN];

	snprintf(str, BUFLEN, "%d,%d\n", cmd, dataLen);
	write(stdinPipe[1], str, strlen(str));
	write(stdinPipe[1], pData, dataLen);
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
		*resultado = lastErrorMessage.c_str();
		return;
	}

	*found = 0;

	if (string(*var_name) == "_r_error") // This was used to get the last error
	{
		*resultado = lastErrorMessage.c_str();
		return;
	}

	writeCommand(CMD_GETVAR, *var_name, strlen(*var_name));

	string line = readLine(resultPipe[0]);
	int resultCode, resultLength;

	if (sscanf(line.c_str(), "%d,%d", &resultCode, &resultLength) != 2)
	{
		*found = 1;
		lastErrorMessage = "Internal error: bad result line";
		*resultado = lastErrorMessage.c_str();
		return;
	}

	// Here, a correct response is a result code of 0, with the variable contents that follows
	if (resultCode < 0)
	{
		*found = 1;
		lastErrorMessage = "Internal error: bad result code";
		*resultado = lastErrorMessage.c_str();
		return;
	}

	variableBuffer.resize(resultLength+1);
	variableBuffer[resultLength] = 0;

	if (resultLength != 0)
		read(resultPipe[0], &(variableBuffer[0]), resultLength);

	*resultado = (char *)&(variableBuffer[0]); // Is it ok to keep reusing this?
}
