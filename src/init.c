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
  
  SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk; // AHB/8 = clksource = 6MHz
  SysTick->LOAD = 6000 & SysTick_LOAD_RELOAD_Msk; //6MHz / 6000 = 1kHz Periode
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk; //Start
}

void init_io(){
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
  GPIO_InitTypeDef gpio_init;
  //PORT A OUTPUTS
  gpio_init.GPIO_Pin = DISP_DB4_PIN | DISP_DB5_PIN| DISP_DB6_PIN| DISP_DB7_PIN;
  gpio_init.GPIO_Pin |= DISP_E_PIN | DISP_RS_PIN | RELAY_SEND_PIN ;
  gpio_init.GPIO_Pin |= RELAY_RECEIVE_PIN;
  gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
  gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
  
  GPIO_Init(GPIOA, &gpio_init); 
    
  //PORT C OUTPUTS
  gpio_init.GPIO_Pin = LED_PIN;
  gpio_init.GPIO_Speed = GPIO_Speed_10MHz;
  gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
  
  GPIO_Init(GPIOC, &gpio_init); 
  
  LED_OFF;
}

void init_usart(){
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
  //Configure USART3
  USART_InitTypeDef usart_init;
  usart_init.USART_BaudRate = 250000;
  usart_init.USART_WordLength = USART_WordLength_8b;
  usart_init.USART_StopBits = USART_StopBits_2;
  usart_init.USART_Parity = USART_Parity_No;
  usart_init.USART_Mode = USART_Mode_Tx;
  usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(USART_OUT, &usart_init);

  //Configure USART1
  usart_init.USART_BaudRate = 115200;   //TODO_ONLY DEBUG //250000
  usart_init.USART_WordLength = USART_WordLength_8b;
  usart_init.USART_StopBits = USART_StopBits_1; //TODO ONLY DEBUG //2
  usart_init.USART_Parity = USART_Parity_No;
  usart_init.USART_Mode = USART_Mode_Rx;
  usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(USART_IN, &usart_init);

  //USART_Cmd(USART_IN, ENABLE);
  USART_Cmd(USART_OUT, ENABLE);
 
  //Configure Pins
  GPIO_InitTypeDef gpio_init;
  gpio_init.GPIO_Pin = GPIO_Pin_10;
  gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
  gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &gpio_init); 
}

void init_dma(){
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  //TX DMA
  DMA_InitTypeDef dma_init;
  dma_init.DMA_PeripheralBaseAddr = (uint32_t)&USART_OUT->DR;
  dma_init.DMA_MemoryBaseAddr = (uint32_t)&dmx.buf;
  dma_init.DMA_DIR = DMA_DIR_PeripheralDST;
  dma_init.DMA_BufferSize = DMX_PACKET_LEN;
  dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable;
  dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  dma_init.DMA_Mode = DMA_Mode_Normal;
  dma_init.DMA_Priority = DMA_Priority_Medium;
  dma_init.DMA_M2M = DMA_M2M_Disable;
  
  DMA_Init(DMA1_Channel2, &dma_init);
  
  //RX DMA
  dma_init.DMA_PeripheralBaseAddr = (uint32_t)&USART_IN->DR;
  dma_init.DMA_MemoryBaseAddr = (uint32_t)&dmx.buf;
  dma_init.DMA_DIR = DMA_DIR_PeripheralSRC;
  dma_init.DMA_BufferSize = DMX_PACKET_LEN;
  dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable;
  dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  dma_init.DMA_Mode = DMA_Mode_Normal;
  dma_init.DMA_Priority = DMA_Priority_Medium;
  dma_init.DMA_M2M = DMA_M2M_Disable;
  
  DMA_Init(DMA1_Channel5, &dma_init);
  
  //Enable Interrupts
  DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);
  DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);

  NVIC_InitTypeDef nvic_init;
  nvic_init.NVIC_IRQChannel = DMA1_Channel2_IRQn;
  nvic_init.NVIC_IRQChannelPreemptionPriority = 10;
  nvic_init.NVIC_IRQChannelSubPriority = 0;
  nvic_init.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_init);
  
  nvic_init.NVIC_IRQChannel = DMA1_Channel5_IRQn;
  NVIC_Init(&nvic_init);
  
  USART_DMACmd(USART_OUT, USART_DMAReq_Tx, ENABLE);
  USART_DMACmd(USART_IN, USART_DMAReq_Rx, ENABLE);
}

void init_timers(){
  //*****************
  //DMX TRANSMIT TIMER
  //*****************

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
 
  TIM_TimeBaseInitTypeDef timer_init;
  //ABP1 48 MHz --> 1 MHz
  timer_init.TIM_Prescaler = 48;
  timer_init.TIM_ClockDivision = TIM_CKD_DIV1;
  timer_init.TIM_CounterMode = TIM_CounterMode_Up;
  //100 MHz
  timer_init.TIM_Period = DMX_PERIOD_US;
  
  TIM_TimeBaseInit(TIM4, &timer_init);
  
  TIM_OCInitTypeDef oc_init;
  oc_init.TIM_OCMode = TIM_OCMode_Active;
  oc_init.TIM_Pulse = DMX_PERIOD_US - DMX_BREAK_TIME_US - DMX_MARK_AFTER_BREAK_US;
  TIM_OC1Init(TIM4, &oc_init);
  
  oc_init.TIM_Pulse = DMX_PERIOD_US - DMX_MARK_AFTER_BREAK_US;
  TIM_OC2Init(TIM4, &oc_init);
  
  //*****************
  //DMX RECEIVE TIMER
  //*****************
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
 
  //ABP1 48 MHz --> 1 MHz
  timer_init.TIM_Prescaler = 48;
  timer_init.TIM_ClockDivision = TIM_CKD_DIV1;
  timer_init.TIM_CounterMode = TIM_CounterMode_Up;
  //100 MHz
  timer_init.TIM_Period = 88-1; //min break time for receiver
  
  TIM_TimeBaseInit(TIM3, &timer_init);  
  
  TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
  //Enable Interrupts in NVIC 
  //timer interrupt enabled in start routine

  NVIC_InitTypeDef nvic_init;
  nvic_init.NVIC_IRQChannel = TIM4_IRQn;
  nvic_init.NVIC_IRQChannelPreemptionPriority = 10;
  nvic_init.NVIC_IRQChannelSubPriority = 0;
  nvic_init.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_init);
  
  nvic_init.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_Init(&nvic_init);
  
}

void init_rx_exti(){
  //Configure ExtInitLine  
  GPIO_InitTypeDef gpio_init;
  
  gpio_init.GPIO_Pin = USART_IN_RX_PIN;
  gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
  gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  
  GPIO_Init(USART_IN_RX_PORT, &gpio_init); 
  
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource10);
  
  EXTI_InitTypeDef exti_init;
  exti_init.EXTI_Line = EXTI_Line10;
  exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
  exti_init.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  exti_init.EXTI_LineCmd = ENABLE;
  
  EXTI_Init(&exti_init); 
  EXTI->IMR &= ~EXTI_Line10;    //Disable Interrupt but keep config

  NVIC_InitTypeDef nvic_init;
  nvic_init.NVIC_IRQChannel = EXTI15_10_IRQn;
  nvic_init.NVIC_IRQChannelPreemptionPriority = 10;
  nvic_init.NVIC_IRQChannelSubPriority = 0;
  nvic_init.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_init);
}

