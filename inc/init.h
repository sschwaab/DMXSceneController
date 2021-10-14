#ifndef INIT_H
#define INIT_H

#ifdef MODULE_INIT
#define VECT_TAB_OFFSET  0x0 

void init_clock();
void init_io();
void init_core();
void init_usart();
void init_timers();
void init_dma();
void init_rx_exti();
void init_mem();

#else

extern void init_clock();
extern void init_io();
extern void init_core();
extern void init_usart();
extern void init_timers();
extern void init_dma();
extern void init_rx_exti();
extern void init_mem();
#endif
#endif