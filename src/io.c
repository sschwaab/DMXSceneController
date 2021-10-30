#define MODULE_IO
#include "project_header.h"

void init_io(){
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  //Necessary for EXTI!!!
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  GPIO_InitTypeDef gpio_init;
  
  gpio_init.GPIO_Pin = LED_USER_PIN;
  gpio_init.GPIO_Speed = GPIO_Speed_10MHz;
  gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
  
  GPIO_Init(LED_USER_PORT, &gpio_init); 
  
  //Init onboard and frontpanel LED (PC13)
  gpio_init.GPIO_Pin = LED_PIN;
  gpio_init.GPIO_Speed = GPIO_Speed_10MHz;
  gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
  
  GPIO_Init(LED_PORT, &gpio_init); 
  
  LED_OFF;
  
  //Init front buttons as floating input (pulled in HW)
  gpio_init.GPIO_Pin = MENU_BUTTON_RECORD_PIN | MENU_BUTTON_DOWN_PIN;
  gpio_init.GPIO_Pin |= MENU_BUTTON_UP_PIN | MENU_BUTTON_EXIT_PIN;
  gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(MENU_BUTTONS_PORT, &gpio_init);
  
  //Enable the corresponding interrupts
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource13);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource15);

  EXTI_InitTypeDef exti_init;
  
  //interrupts in core can not be initialized combined
  exti_init.EXTI_Line = EXTI_Line12;
  exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
  exti_init.EXTI_Trigger = EXTI_Trigger_Rising;
  exti_init.EXTI_LineCmd = ENABLE;
  EXTI_Init(&exti_init); 
  
  exti_init.EXTI_Line = EXTI_Line13;
  exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
  exti_init.EXTI_Trigger = EXTI_Trigger_Rising;
  exti_init.EXTI_LineCmd = ENABLE;
  EXTI_Init(&exti_init); 
  
  exti_init.EXTI_Line = EXTI_Line14;
  exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
  exti_init.EXTI_Trigger = EXTI_Trigger_Rising;
  exti_init.EXTI_LineCmd = ENABLE;
  EXTI_Init(&exti_init); 
  
  exti_init.EXTI_Line = EXTI_Line15;
  exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
  exti_init.EXTI_Trigger = EXTI_Trigger_Rising;
  exti_init.EXTI_LineCmd = ENABLE;
  EXTI_Init(&exti_init); 
  
  NVIC_InitTypeDef nvic_init;
  
  nvic_init.NVIC_IRQChannel = EXTI15_10_IRQn;
  nvic_init.NVIC_IRQChannelPreemptionPriority = 10;
  nvic_init.NVIC_IRQChannelSubPriority = 0;
  nvic_init.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_init);
}