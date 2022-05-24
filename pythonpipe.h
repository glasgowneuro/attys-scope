#pragma once

#include<stdio.h>
#include<string>

#ifdef _WIN32
#include <windows.h> 
#include <tchar.h>
#include <strsafe.h>
#endif

struct PythonPipe {
#ifdef _WIN32
#define BUFSIZE 4096 

	HANDLE g_hChildStd_IN_Rd = NULL;
	HANDLE g_hChildStd_IN_Wr = NULL;
	HANDLE g_hChildStd_OUT_Rd = NULL;
	HANDLE g_hChildStd_OUT_Wr = NULL;

	HANDLE g_hInputFile = NULL;

	int startWin();
#else
	FILE* pipe = nullptr;
#endif
	std::string filename;
	int start(std::string theFilename);
	void stop();
	void write(const char* data);
	bool hasError = false;
};
