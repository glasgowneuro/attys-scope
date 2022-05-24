#pragma once

#include<stdio.h>
#include<string>

struct PythonPipe {
	FILE* pipe = nullptr;
	std::string filename;
	int start(std::string theFilename);
	void stop();
};
