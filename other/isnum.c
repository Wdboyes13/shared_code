#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef enum { FLOAT, DEC, INVALID, HEX, OCT, BIN } numtype;

numtype isanumber(const char* str) {
    if (!str || strlen(str) == 0) {
        return INVALID;
    }

    numtype n = DEC;

    if (str[0] == '-') {
        str++;
        if (*str == '\0')
            return INVALID;
    }

    // Check for prefixes
    if (str[0] == '0' && strlen(str) > 1) {
        switch (str[1]) {
            case 'x':
            case 'X':
                n = HEX;
                str += 2;
                if (*str == '\0')
                    return INVALID;
                break;
            case 'o':
            case 'O':
                n = OCT;
                str += 2;
                if (*str == '\0')
                    return INVALID;
                break;
            case 'b':
            case 'B':
                n = BIN;
                str += 2;
                if (*str == '\0')
                    return INVALID;
                break;
            default:
                n = OCT;
                str += 1;
        }
    }

    int has_dot = 0;
    while (*str != '\0') {
        char tchar = *str;

        if (n == HEX) {
            if (!isxdigit(tchar))
                return INVALID;
        } else if (n == BIN) {
            if (tchar != '0' && tchar != '1')
                return INVALID;
        } else if (n == OCT) {
            if (tchar >= '0' && tchar <= '7') {
                // Valid octal digit, stay OCT
            } else if (tchar >= '8' && tchar <= '9') {
                // Invalid octal, convert to DEC
                n = DEC;
            } else if (tchar == '.') {
                // Octal with decimal point becomes float
                if (has_dot)
                    return INVALID;
                has_dot = 1;
                n = FLOAT;
            } else {
                return INVALID;
            }
        } else { // DEC or FLOAT
            if (tchar == '.') {
                if (has_dot)
                    return INVALID;
                has_dot = 1;
                n = FLOAT;
            } else if (!isdigit(tchar)) {
                return INVALID;
            }
        }

        str++;
    }

    return n;
}
