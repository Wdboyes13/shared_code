#pragma once
#ifdef DLL
#    if defined(_WIN32) || defined(_WIN64)
#        define API __declspec(dllexport)
#    else
#        define API
#    endif
#else
#    if defined(_WIN32) || defined(_WIN64)
#        define API __declspec(dllimport)
#    else
#        define API
#    endif
#endif

API const char* getprogrampath();
