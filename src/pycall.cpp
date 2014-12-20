#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include "pycontroller.h"

using namespace std;

typedef map<string, PyController *> PyCtrlMap;

static string messageBuffer;
static vector<uint8_t> variableBuffer;
static PyCtrlMap pythonInstances;
static string scriptName;

extern "C" 
{
	void py_init(const char **script);
	void py_close();
	void py_exec_code(const char** code, const char **instancename, int* exit_status, char **message);
	void py_get_var(const char** var_name, const char **instancename, int* found, char** resultado);
	void py_set_exec(const char** executable, const char **instancename);
}

void py_init(const char **script)
{
	if (*script)
		scriptName = string(*script);
}

PyController *getNamedInstance(const string &name)
{
	PyCtrlMap::iterator it = pythonInstances.find(name);
	PyController *pInstance = 0;

	if (it == pythonInstances.end())
	{
		pInstance = new PyController(name);

		if (scriptName.length() > 0)
			pInstance->setPythonScript(scriptName);

		pythonInstances[name] = pInstance;
	}
	else
		pInstance = it->second;

	return pInstance;
}

void py_set_exec(const char** executable, const char **instancename)
{
	if (*executable)
	{
		string name = "";
		if (instancename)
			name = string(*instancename);

		PyController *pPyController = getNamedInstance(name);
		pPyController->setPythonExecutable(*executable);
	}
}

void py_close()
{
	for (map<string, PyController *>::iterator it = pythonInstances.begin() ; it != pythonInstances.end() ; ++it)
		delete it->second;

	pythonInstances.clear();
}

void py_exec_code(const char** code, const char **instancename, int* exit_status, char **message)
{
	if (!*code)
		return;

	string name = "";
	if (instancename)
		name = string(*instancename);

	PyController *pPyController = getNamedInstance(name);

	if (!pPyController->exec(*code))
	{
		messageBuffer = pPyController->getErrorString();
		*message = (char *)messageBuffer.c_str();
		*exit_status = -1;
		return;
	}
	
	*exit_status = 0; // No error
}

void py_get_var(const char** var_name, const char **instancename, int* found, char** result )
{
	if (!*var_name)
		return;

	string name = "";
	if (instancename)
		name = string(*instancename);

	PyController *pPyController = getNamedInstance(name);

	if (!pPyController->getVariable(*var_name, variableBuffer))
	{
		messageBuffer = pPyController->getErrorString();
		*result = (char *)messageBuffer.c_str();
		*found = 0;
		return;
	}

	*result = (char *)&(variableBuffer[0]); // Is it ok to keep reusing this?
	*found = 1;
}

