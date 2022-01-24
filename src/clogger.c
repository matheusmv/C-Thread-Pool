#include "clogger.h"

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define true  1
#define false 0

#define ACTIVE   true
#define INACTIVE false

typedef struct clogger {
        FILE            *output;
        int8_t          colored;
        int8_t          initialized;
        pthread_mutex_t mutex;
} clogger_t;

static clogger_t prv_logger;

static void
clogger_mutex_init(clogger_t *clogger)
{
        pthread_mutex_init(&clogger->mutex, NULL);
}

static void
clogger_mutex_lock(clogger_t *clogger)
{
        pthread_mutex_lock(&clogger->mutex);
}

static void
clogger_mutex_unlock(clogger_t *clogger)
{
        pthread_mutex_unlock(&clogger->mutex);
}

static clogger_t
clogger_create(void)
{
        clogger_t new_logger = (clogger_t) {
                .output         = stderr,
                .colored        = INACTIVE,
                .initialized    = true
        };

        clogger_mutex_init(&new_logger);

        return new_logger;
}

static void
clogger_log(FILE *stream, const char *time, const char *log_level,
            const char *filename, const char *function, int line,
            const char *format, va_list args)
{
        char message[MSGMAXLENGTH];

        vsnprintf(message, sizeof(message), format, args);
        fprintf(stream, "%s %s %s:'%s':%d - %s\n", time, log_level, filename,
                function, line, message);
}

static char *
log_type_to_string(log_type_t type, int8_t colored)
{
        switch (type) {
        case INFO:
                return colored ? "\x1B[32mINFO\033[0m " : "INFO ";
        case DEBUG:
                return colored ? "\x1B[34mDEBUG\033[0m" : "DEBUG";
        case WARN:
                return colored ? "\x1B[33mWARN\033[0m " : "WARN ";
        case ERROR:
                return colored ? "\x1B[31mERROR\033[0m" : "ERROR";
        default:
                return "UNDEF";
        }
}

static void
clogger_init(void)
{
        prv_logger = clogger_create();
        clogger_mutex_lock(&prv_logger);

#if defined(LCOLOR)
        prv_logger.colored = ACTIVE;
#endif

        clogger_mutex_unlock(&prv_logger);
}

static void
get_time(char *dest, size_t dest_size)
{
        time_t seconds;
        struct tm time_info;

        seconds = time(NULL);
#if defined(_POSIX_SOURCE)
        tzset();
        localtime_r(&seconds, &time_info);
#else
        time_info = *localtime(&seconds);
#endif
        strftime(dest, dest_size, "%b %d %Y %X", &time_info);
}

void clogger(log_type_t type, const char *filename, const char *function,
             int line, const char *format, ...)
{
        if (!prv_logger.initialized) {
                clogger_init();
        }

        va_list args;

        char date_time[32];

        clogger_mutex_lock(&prv_logger);

        va_start(args, format);

        get_time(date_time, sizeof(date_time));

        char *log_type_str = log_type_to_string(type, prv_logger.colored);
        clogger_log(prv_logger.output, date_time, log_type_str, filename,
                    function, line, format, args);

        va_end(args);

        clogger_mutex_unlock(&prv_logger);
}

void clogger_f(log_type_t type, const char *filepath, const char *filename,
               const char *function, int line, const char *format, ...)
{
        if (!prv_logger.initialized) {
                clogger_init();
        }

        va_list args;

        char date_time[32];

        clogger_mutex_lock(&prv_logger);

        FILE *file = fopen(filepath, "a");
        if (file == NULL) {
                fprintf(stderr, "%s - %s\n", strerror(errno), filepath);
                clogger_mutex_unlock(&prv_logger);
                return;
        }

        va_start(args, format);

        get_time(date_time, sizeof(date_time));

        char *log_type_str = log_type_to_string(type, INACTIVE);
        clogger_log(file, date_time, log_type_str, filename, function, line,
                    format, args);

        va_end(args);

        fclose(file);

        clogger_mutex_unlock(&prv_logger);
}
