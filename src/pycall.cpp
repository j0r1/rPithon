#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>
#include "pycontroller.h"

static PyController *pPyController = 0;
static std::string messageBuffer;
static std::vector<uint8_t> variableBuffer;

extern "C" 
{
	void py_init(const char **script);
	void py_close();
	void py_exec_code(const char** code, int* exit_status, char **message);
	void py_get_var(const char** var_name, int* found, char** resultado);
	void py_set_exec(const char** executable);
}

void py_init(const char **script)
{
	pPyController = new PyController();
	if (*script)
		pPyController->setPythonScript(*script);
}

void py_set_exec(const char** executable)
{
	if (*executable)
		pPyController->setPythonExecutable(*executable);
}

void py_close()
{
	delete pPyController;
	pPyController = 0;
}

void py_exec_code(const char** code, int* exit_status, char **message )
{
	if (!*code)
		return;

	if (!pPyController->exec(*code))
	{
		messageBuffer = pPyController->getErrorString();
		*message = (char *)messageBuffer.c_str();
		*exit_status = -1;
		return;
	}
	
	*exit_status = 0; // No error
}

void py_get_var( const char** var_name, int* found, char** resultado )
{
	if (!*var_name)
		return;

	if (!pPyController->getVariable(*var_name, variableBuffer))
	{
		messageBuffer = pPyController->getErrorString();
		*found = 0;
		return;
	}

	*resultado = (char *)&(variableBuffer[0]); // Is it ok to keep reusing this?
	*found = 1;
}

