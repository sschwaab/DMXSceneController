#ifndef STATES_H
#define STAES_H

#ifdef MODULE_STATES
void fsm();
#else
extern void fsm();
#endif
#endif