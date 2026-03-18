#include <stdio.h>
#include "progname.h"

int main() {
    const char* progpath = getprogrampath();
    if (progpath != NULL) {
        printf("%s\n", progpath);
    } else {
        perror("getprogrampath");
        return 1;
    }
    return 0;
}
