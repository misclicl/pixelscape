#pragma once

enum LogLevel {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
};

// Initializes the logger. Should be called before any logging operations.
void logger_init(LogLevel level);

// Logs a message with the given level.
void log_message(LogLevel level, const char *format, ...);
