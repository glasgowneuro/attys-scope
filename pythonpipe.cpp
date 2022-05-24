#include "pythonpipe.h"

int PythonPipe::start(std::string theFilename) {
	filename = theFilename;
	filename = "python " + filename;
#ifdef _WIN32
    return startWin();
#else
	pipe = popen(filename.c_str(), "w");
    if (nullptr == pipe) return errno;
#endif
	return 0;
}

void PythonPipe::stop() {
#ifdef _WIN32
    CloseHandle(g_hChildStd_IN_Wr);
#else
    pclose(pipe);
#endif
}

void PythonPipe::write(const char* data)
{
#ifdef _WIN32
    DWORD dwWritten;
    BOOL bSuccess = FALSE;

    bSuccess = WriteFile(g_hChildStd_IN_Wr, data, strlen(data), &dwWritten, NULL);
    if (!bSuccess) {
        hasError = true;
    }
#endif
}

#ifdef _WIN32

int PythonPipe::startWin() {
    SECURITY_ATTRIBUTES saAttr;

    fprintf(stderr,"\n->Start of parent execution.\n");

    // Set the bInheritHandle flag so pipe handles are inherited. 

    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Create a pipe for the child process's STDOUT. 

    if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0)) {
        fprintf(stderr,"StdoutRd CreatePipe");
        return -1;
    }

    // Ensure the read handle to the pipe for STDOUT is not inherited.

    if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) {
        fprintf(stderr, "Stdout SetHandleInformation");
        return -1;
    }

    // Create a pipe for the child process's STDIN. 

    if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0)) {
        fprintf(stderr, "Stdin CreatePipe");
        return -1;
    }

    // Ensure the write handle to the pipe for STDIN is not inherited. 

    if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0)) {
        fprintf(stderr, "Stdin SetHandleInformation");
        return -1;
    }

    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;
    BOOL bSuccess = FALSE;

    // Set up members of the PROCESS_INFORMATION structure. 

    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

    // Set up members of the STARTUPINFO structure. 
    // This structure specifies the STDIN and STDOUT handles for redirection.

    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = g_hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
    siStartInfo.hStdInput = g_hChildStd_IN_Rd;
    siStartInfo.wShowWindow = SW_HIDE;
    siStartInfo.dwFlags |= (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);

    const size_t newsizew = filename.size() + 1;
    size_t convertedChars = 0;
    wchar_t* wcstring = new wchar_t[newsizew];
    mbstowcs_s(&convertedChars, wcstring, newsizew, filename.c_str(), _TRUNCATE);

    bSuccess = CreateProcess(NULL,
        wcstring,      // command line 
        NULL,          // process security attributes 
        NULL,          // primary thread security attributes 
        TRUE,          // handles are inherited 
        CREATE_NEW_CONSOLE,             // creation flags 
        NULL,          // use parent's environment 
        NULL,          // use parent's current directory 
        &siStartInfo,  // STARTUPINFO pointer 
        &piProcInfo);  // receives PROCESS_INFORMATION 

    delete[]wcstring;

     // If an error occurs, exit the application. 
    if (!bSuccess)
        return -1;
    else
    {
        // Close handles to the child process and its primary thread.
        // Some applications might keep these handles to monitor the status
        // of the child process, for example. 

        CloseHandle(piProcInfo.hProcess);
        CloseHandle(piProcInfo.hThread);

        // Close handles to the stdin and stdout pipes no longer needed by the child process.
        // If they are not explicitly closed, there is no way to recognize that the child process has ended.

        CloseHandle(g_hChildStd_OUT_Wr);
        CloseHandle(g_hChildStd_IN_Rd);
    }


    return 0;
}

#endif
