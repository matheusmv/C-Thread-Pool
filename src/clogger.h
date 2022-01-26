#ifndef CLOGGER_H
#define CLOGGER_H

#include <stdio.h>

#define MSGMAXLENGTH 512

typedef enum log_type {
        INFO,
        DEBUG,
        WARN,
        ERROR
} log_type_t;

#if defined(NCLOG)
#define LOG_INFO(FORMAT, ...)
#define LOG_DEBUG(FORMAT, ...)
#define LOG_WARNING(FORMAT, ...)
#define LOG_ERROR(FORMAT, ...)
#else
#define LOG_INFO(FORMAT, ...) clogger(INFO, __FILE__, __func__, __LINE__, FORMAT, ##__VA_ARGS__)
#define LOG_DEBUG(FORMAT, ...) clogger(DEBUG, __FILE__, __func__, __LINE__, FORMAT, ##__VA_ARGS__)
#define LOG_WARNING(FORMAT, ...) clogger(WARN, __FILE__, __func__, __LINE__, FORMAT, ##__VA_ARGS__)
#define LOG_ERROR(FORMAT, ...) clogger(ERROR, __FILE__, __func__, __LINE__, FORMAT, ##__VA_ARGS__)
#endif /* NCLOG */

#define LOG_INFO_F(FILEPATH, FORMAT, ...) clogger_f(INFO, FILEPATH, __FILE__, __func__, __LINE__, FORMAT, ##__VA_ARGS__)
#define LOG_DEBUG_F(FILEPATH, FORMAT, ...) clogger_f(DEBUG, FILEPATH, __FILE__, __func__, __LINE__, FORMAT, ##__VA_ARGS__)
#define LOG_WARNING_F(FILEPATH, FORMAT, ...) clogger_f(WARN, FILEPATH, __FILE__, __func__, __LINE__, FORMAT, ##__VA_ARGS__)
#define LOG_ERROR_F(FILEPATH, FORMAT, ...) clogger_f(ERROR, FILEPATH, __FILE__, __func__, __LINE__, FORMAT, ##__VA_ARGS__)

void clogger(log_type_t type, const char *filename, const char *function,
             int line, const char *format, ...);
void clogger_f(log_type_t type, const char *filepath, const char *filename,
               const char *function, int line, const char *format, ...);

#endif /* CLOGGER_H */
