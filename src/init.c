#define MODULE_INIT
#include "project_header.h"

void init_core(){
  RCC->CIR = 0x009F0000;
  SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH. */
}

void init_clock(){
  RCC_HSEConfig(RCC_HSE_ON);
  while(RCC_WaitForHSEStartUp()==ERROR);
  RCC_PLLCmd(DISABLE);
  
  FLASH->ACR |= FLASH_ACR_LATENCY_1;
  
  //8MHZ/1*6=48MHz
  RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_6);
  RCC_PLLCmd(ENABLE);
  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
  
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  //RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);
  
  //HCLK = 48MHz
  RCC_HCLKConfig(RCC_SYSCLK_Div1);
  //PCLK1 = 24MHz
  RCC_PCLK1Config(RCC_HCLK_Div2);
  //PCLK2 = 48MHz
  RCC_PCLK2Config(RCC_HCLK_Div1);
  
  //Init systick to 1ms
  SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk; // AHB/8 = clksource = 6MHz
  SysTick->LOAD = 6000 & SysTick_LOAD_RELOAD_Msk; //6MHz / 6000 = 1kHz Periode
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk; //Start
}