#ifndef MAIN_H
#define MAIN_H

#define LED_IS_OFF GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13)
#define LED_ON  GPIO_ResetBits(LED_PORT, LED_PIN)
#define LED_OFF GPIO_SetBits(LED_PORT, LED_PIN)
   

#ifdef MODULE_MAIN
//Global Variables
uint64_t millis;
//|-----------------------------------------------------------------------|
//|   B7   |   B6   |   B5   |   B4   |   B3   |   B2   |   B1   |   B0   |
//|-----------------------------------------------------------------------|
uint8_t recall_buttons;

//|-----------------------------------------------------------------------|
//|   XX   |   XX   |   XX   |   XX   |   B3   |   B2   |   B1   |   B0   |
//|-----------------------------------------------------------------------|
uint8_t menu_buttons;


uint8_t main();
void wait_ms(uint16_t ms);


#else
extern uint64_t millis;
extern void wait_ms(uint16_t ms);
extern uint8_t recall_buttons;
extern uint8_t menu_buttons;

#endif
#endif