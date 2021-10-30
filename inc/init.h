#ifndef INIT_H
#define INIT_H

#ifdef MODULE_INIT
#define VECT_TAB_OFFSET  0x0 

void init_clock();
void init_core();
#else

extern void init_clock();
extern void init_core();
#endif
#endif