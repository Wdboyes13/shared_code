#define DLL

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

#include "../include/progname.h"

#if defined(__APPLE__)
#    include <mach-o/dyld.h>
#elif defined(__linux__)
#    include <unistd.h>
#elif defined(_WIN32) || defined(_WIN64)
#    include <windows.h>
#endif

API const char* getprogrampath() {
#if !defined(_WIN32) && !defined(_WIN64)
    uint32_t sz = PATH_MAX;
    char* buffer = malloc(sz);
#    if defined(__APPLE__)
    if (!_NSGetExecutablePath(buffer, &sz)) {
        return buffer;
    } else {
        return NULL;
    }
#    elif defined(__linux__)
    if (realpath("/proc/self/exe", buffer) != NULL) {
        return buffer;
    } else {
        return NULL;
    }
#    endif
#else
#    ifdef UNICODE
    LPWSTR buffer = malloc(sizeof(WCHAR) * MAX_PATH);
#    else
    LPSTR buffer = malloc(MAX_PATH);
#    endif
    if (GetModuleFileName(NULL, buffer, MAX_PATH)) {
        return buffer;
    } else {
        return NULL;
    }
#endif
}
