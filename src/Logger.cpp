#include "Logger.h"

const char* Logger::LOG_TOPIC_PREFIX = "devices";

const char* Logger::getLogLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

bool Logger::isLogLevelEnabled(LogLevel level) {
    return static_cast<int>(level) >= static_cast<int>(logLevel);
}

void Logger::log(LogLevel level, const char* source, const char* message) {
    if (static_cast<int>(logLevel) < 0 || static_cast<int>(logLevel) > getLogLevelCount()-1) {
        char msgBuffer[64];
        snprintf(msgBuffer, sizeof(msgBuffer), "Invalid log level: '%d' - changing to default: INFO", logLevel);
        logLevel = LogLevel::INFO;
        info("Logger", msgBuffer);
    }

    if (!isLogLevelEnabled(level)) {
        return;
    }

    unsigned long ms = millis();

    char msgBuffer[256];
    snprintf(msgBuffer, sizeof(msgBuffer), "[%lu] %s: %s: %s", ms, getLogLevelString(level), source, message);

    Serial.println(msgBuffer);
}

void Logger::debug(const char* source, const char* message) {
    log(LogLevel::DEBUG, source, message);
}

void Logger::info(const char* source, const char* message) {
    log(LogLevel::INFO, source, message);
}

void Logger::warning(const char* source, const char* message) {
    log(LogLevel::WARNING, source, message);
}

void Logger::error(const char* source, const char* message) {
    log(LogLevel::ERROR, source, message);
}