#ifndef CWPLATFORM_H
#define CWPLATFORM_H

struct CwCmd;
struct CwStr;
struct CwArena;

void cwplatform_panic();
void cwplatform_print(struct CwStr message);
void cwplatform_log(struct CwStr message);

int cwplatform_run(struct CwCmd cmd);

void cwplatform_sleepms(int ms);

#endif
