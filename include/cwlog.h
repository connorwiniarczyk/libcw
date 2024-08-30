#ifndef CWLOG_H
#define CWLOG_H

#include "cwstring.h"

enum CwLogLevel {
    CWLOG_DEBUG,
    CWLOG_INFO,
    CWLOG_WARNING,
    CWLOG_ERROR,
};

typedef struct CwLogger {
    CwString* buffer;
    void* data;
    int (*flush)(struct CwLogger* self, int level);
} CwLogger;

CwLogger* cwlogger_standard(enum CwLogLevel filter);

int cwlog(CwLogger* self, enum CwLogLevel level, const char* message);

#endif

#ifdef CWLOG_IMPLEMENTATION

int cwlog(CwLogger* self, enum CwLogLevel level, const char* message) {
    if (self == NULL) return 0;

    cwstring_clear(self -> buffer);
    cwstring_append_cstr(self -> buffer, message);
    (self -> flush)(self, level);
    return 0;
}


static int standard_flush(CwLogger* self, int level) {
    switch (level) {
        case CWLOG_DEBUG:   fprintf(stderr, "DEBUG: "); break; 
        case CWLOG_INFO:    fprintf(stderr, "INFO: "); break; 
        case CWLOG_WARNING: fprintf(stderr, "WARNING: "); break; 
        case CWLOG_ERROR:   fprintf(stderr, "ERROR: "); break; 
        default: break;
    }

    cwstring_push(self -> buffer, '\0');

    fprintf(stderr, self -> buffer -> ptr);
    fprintf(stderr, "\n");

    cwstring_clear(self -> buffer);
    return 0;
}

CwLogger* cwlogger_standard(enum CwLogLevel filter) {
    CwLogger* output = malloc(sizeof(CwLogger));
    if (output == NULL) return NULL; 

    output -> buffer = cwstring_new();
    if (output -> buffer == NULL) { free(output); return NULL; }

    output -> data = malloc(sizeof(enum CwLogLevel));
    if (output -> data == NULL) { cwstring_free(output -> buffer); free(output); return NULL; }

    output -> flush = &standard_flush;
    return output;
}

#endif
