#ifndef PYCONTROLLER_H

#define PYCONTROLLER_H

#include <stdint.h>
#include <string>
#include <vector>

class PyController
{
public:
	PyController();
	~PyController();

	const std::string &getErrorString() const							{ return m_lastError; }

	void setPythonExecutable(const std::string &pythonExec)						{ m_pythonExecutable = pythonExec; }
	void setPythonScript(const std::string &pythonScript)						{ m_scriptPath = pythonScript; }

	bool exec(const std::string &code);
	bool getVariable(const std::string &name, std::vector<uint8_t> &buffer);
private:
	void setErrorString(const std::string &s) const							{ m_lastError = s; }
	void cleanup();

	mutable std::string m_lastError;

	std::string m_pythonExecutable;
	std::string m_scriptPath;
	bool m_startTried;
#ifdef WIN32
#else
	bool checkRunning();
	bool readLine(std::string &line);
	void writeCommand(int cmd, const void *pData, int dataLen);

	int m_stdinPipe[2];
	int m_resultPipe[2];
	int m_pythonPID;

#endif // WIN32
};

#endif // PYCONTROLLER_H

