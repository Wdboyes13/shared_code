#pragma once

#if __cplusplus < 202302L
#    error "This header will only work with C++23"
#endif

#include <format>
#include <fstream>
#include <iostream>
#include <print>
#include <string>

#define DEFFMT(RET, NAME)      \
    template<typename... Args> \
    static inline RET NAME(std::format_string<Args...> fmt, Args&&... args)

#define DEFFILFMT(RET, NAME, FT)                                \
    template<typename... Args>                                  \
    static inline RET NAME(FT, std::format_string<Args...> fmt, \
                           Args&&... args)

#define FORMAT std::format(fmt, std::forward<Args>(args)...)

namespace printer {

using std::print;
using std::println;

namespace file {

// I'll work on WinAPI functions later, though for now I'm to lazy lmao
#ifndef _WIN32
#    include <unistd.h>
#    include <cstdio>

// Print to file desc., returns -1 on error 0 on success
DEFFILFMT(int, fdprint, int fd) {
    int newfd = dup(fd);
    if (newfd == -1)
        return -1;

    FILE* f = fdopen(newfd, "w");
    if (!f) {
        close(newfd);
        return -1;
    }

    std::print(f, "{}", FORMAT);

    fclose(f);
    return 0;
}

// Print line to file desc., returns -1 on error 0 on success
DEFFILFMT(int, fdprintln, int fd) { return fdprint(fd, "{}\n", FORMAT); }
#endif

// Print to std::fstream
DEFFILFMT(void, fprint, std::fstream& file) { std::print(file, "{}", FORMAT); }

// Print line to std::fstream
DEFFILFMT(void, fprintln, std::fstream& file) { fprint(file, "{}", FORMAT); }

// Print to C file pointer
DEFFILFMT(void, fpprint, FILE* file) { std::print(file, "{}", FORMAT); }

// Print line to C file pointer
DEFFILFMT(void, fpprintln, FILE* file) { fpprint(file, "{}", FORMAT); }

} // namespace file

// Print to standard error (stderr)
DEFFMT(void, eprint) { std::print(std::cerr, "{}", FORMAT); }

// Print line to standard error (stderr)
DEFFMT(void, eprintln) { eprint("{}\n", FORMAT); }

// Input
DEFFMT(std::string, input) {
    std::print("{}", FORMAT);
    std::string str;
    std::getline(std::cin, str);
    return str;
}

} // namespace printer

#undef DEFFMT
#undef DEFFILFMT
#undef FORMAT
