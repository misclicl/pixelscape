#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include "logger.h"

static LogLevel log_level = LogLevel::LOG_LEVEL_INFO;

static const char* level_to_string(LogLevel level) {
    switch (level) {
        case LOG_LEVEL_DEBUG: return "DEBUG";
        case LOG_LEVEL_INFO: return "INFO";
        case LOG_LEVEL_WARN: return "WARN";
        case LOG_LEVEL_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

void logger_init(LogLevel default_log_level) {
    log_level = default_log_level;
}


void log_message(LogLevel level, const char *format, ...) {
    if (level < log_level) return;  // Skip logs below the current log level

    char buffer[512];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // Get current time for timestamp
    time_t rawtime;
    struct tm * timeinfo;
    char timestamp[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);

    printf("[%s][%s]: %s\n", timestamp, level_to_string(level), buffer);
}