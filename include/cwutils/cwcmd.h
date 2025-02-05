#ifndef CWCMD_H
#define CWCMD_H

#include <cwutils/cwarena.h>

typedef struct CwCmd {
    const char **ptr;
    int size;
    CwArena memory;
} CwCmd;

CwCmd cwcmd_create(CwArena a, const char* cmd);

void cwcmd_push_arg(CwCmd* self, const char* arg);
int  cwcmd_run(CwCmd* self);

#endif
