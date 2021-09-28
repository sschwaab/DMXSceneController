#ifndef MAIN_H
#define MAIN_H

#define LED_IS_OFF GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13)
#define LED_ON  GPIO_ResetBits(LED_PORT, LED_PIN)
#define LED_OFF GPIO_SetBits(LED_PORT, LED_PIN)
   

#ifdef MODULE_MAIN

uint8_t main();
void wait_ms(uint16_t ms);

uint64_t millis;

#else
extern uint64_t millis;
extern void wait_ms(uint16_t ms);

#endif
#endif