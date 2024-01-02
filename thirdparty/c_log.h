/*
MIT License

Copyright (c) 2024 jarvarvarvis

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef _C_LOG_HEADER
#define _C_LOG_HEADER

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum {
    C_LOG_SEVERITY_DEBUG   = 1,
    C_LOG_SEVERITY_INFO    = 2,
    C_LOG_SEVERITY_WARNING = 3,
    C_LOG_SEVERITY_ERROR   = 4,

    C_LOG_SEVERITY_SHOW_NONE = 5,
} CLogSeverity;

#ifndef C_LOG_MIN_SEVERITY
#define C_LOG_MIN_SEVERITY C_LOG_SEVERITY_DEBUG
#endif


#define C_LOG_ANSI_RESET       "\033[0m"

#define C_LOG_ANSI_GREY        "\033[90m"

#define C_LOG_ANSI_BOLD_WHITE  "\033[97;1m"
#define C_LOG_ANSI_BOLD_RED    "\033[91;1m"
#define C_LOG_ANSI_BOLD_GREEN  "\033[92;1m"
#define C_LOG_ANSI_BOLD_YELLOW "\033[93;1m"

bool c_log_should_show_severity(CLogSeverity severity) {
    return severity >= C_LOG_MIN_SEVERITY;
}

int c_log(CLogSeverity severity, const char* format, ...) {
    assert(severity != C_LOG_SEVERITY_SHOW_NONE);
    char temp[256];

    // Format the message into a local buffer
    va_list args;
    va_start(args, format);
    int length = vsnprintf(temp, sizeof(temp), format, args);
    va_end(args);

    if (length < 0) {
        // Encoding error
        return -1;
    }

    bool reformat = false;
    char *message_str;
    if (length < sizeof(temp)) {
        // The formatted message fit into the buffer
        // -> Create a copy
        message_str = strdup(temp);
    } else {
        // Message was truncated
        // -> Allocate a large enough buffer to fit the entire string
        message_str = (char*) malloc(length + 1);
        reformat = true;
    }

    if (!message_str) {
        // Allocation error
        return -1;
    }

    if (reformat) {
        // Format the message again
        va_start(args, format);
        length = vsnprintf(message_str, length + 1, format, args);
        va_end(args);
    }

    // Get current time
    time_t timer = time(NULL);
    struct tm *time_info = localtime(&timer);

    char time_str[35];
    memset(time_str, 0, 35);
    strftime(time_str, 35, C_LOG_ANSI_GREY "%Y-%m-%d %H:%M:%S" C_LOG_ANSI_RESET,
            time_info);

    // Show log message
    #define show_sev(sev) (severity == (sev) && c_log_should_show_severity(sev))

    if (show_sev(C_LOG_SEVERITY_DEBUG)) {
        const char *severity_str = C_LOG_ANSI_BOLD_GREEN "debug" C_LOG_ANSI_RESET;
        printf("%s  [%s] %s\n", time_str, severity_str, message_str);
    }

    else if (show_sev(C_LOG_SEVERITY_INFO)) {
        const char *severity_str = C_LOG_ANSI_BOLD_WHITE "info" C_LOG_ANSI_RESET;
        printf("%s  [%s] %s\n", time_str, severity_str, message_str);
    }

    else if (show_sev(C_LOG_SEVERITY_WARNING)) {
        const char *severity_str = C_LOG_ANSI_BOLD_YELLOW "warning" C_LOG_ANSI_RESET;
        printf("%s  [%s] %s\n", time_str, severity_str, message_str);
    }

    else if (show_sev(C_LOG_SEVERITY_ERROR)) {
        const char *severity_str = C_LOG_ANSI_BOLD_RED "error" C_LOG_ANSI_RESET;
        printf("%s  [%s] %s\n", time_str, severity_str, message_str);
    }

    #undef  show_sev

    free(message_str);
    return length;
}

#endif /* _C_LOG_HEADER */
