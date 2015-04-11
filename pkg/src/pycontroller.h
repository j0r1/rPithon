#ifndef PYCONTROLLER_H

#define PYCONTROLLER_H

#ifdef WIN32
#include <windows.h>
#endif // WIN32

#include <stdint.h>
#include <string>
#include <vector>

class PyController
{
public:
	PyController(const std::string &identifier);
	~PyController();

	const std::string &getErrorString() const									{ return m_lastError; }

	void setPythonExecutable(const std::string &pythonExec)						{ m_pythonExecutable = pythonExec; }
	void setPythonScript(const std::string &pythonScript)						{ m_scriptPath = pythonScript; }

	bool exec(const std::string &code);
	bool getVariable(const std::string &name, std::vector<uint8_t> &buffer);

	static void setDefaultPythonExecutable(const std::string &pythonExec)		{ s_defaultPythonExecutable = pythonExec; }
private:
	void setErrorString(const std::string &s) const								{ m_lastError = s; }
	void cleanup();
	bool checkRunning();
	bool readLine(std::string &line);
	void writeCommand(int cmd, const void *pData, int dataLen);
	void startupMessage();

	mutable std::string m_lastError;

	const std::string m_identifier;
	std::string m_pythonExecutable;
	std::string m_scriptPath;
	bool m_startTried;
#ifdef WIN32
	HANDLE m_hStdinPipe[2];
	HANDLE m_hResultPipe[2];
	HANDLE m_hPyProcess;
	HANDLE m_hPyThread;
#else
	int m_stdinPipe[2];
	int m_resultPipe[2];
	int m_pythonPID;
#endif // WIN32

	static std::string s_defaultPythonExecutable;
};

#endif // PYCONTROLLER_H

