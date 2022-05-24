#include "pythonpipe.h"

int PythonPipe::start(std::string theFilename) {
	filename = theFilename;
	filename = "python " + filename;
#ifdef _WIN32
	pipe = _popen(filename.c_str(), "w");
#else
	pipe = popen(filename.c_str(), "w");
#endif
	if (nullptr == pipe) return errno;
	return 0;
}

void PythonPipe::stop() {
#ifdef _WIN32
		_pclose(pipe);
#else
		pclose(pipe);
#endif
}
