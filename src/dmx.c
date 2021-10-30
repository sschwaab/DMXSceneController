#define MODULE_DMX
#include "project_header.h"

void init_dmx(void){
  //------------------------------------------------------------------
  //---Init GPIO Pins for Relays and Enables for RS485 Transceivers---
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

  GPIO_InitTypeDef gpio_init;
  
  gpio_init.GPIO_Pin = RELAY_SEND_PIN | RELAY_RECEIVE_PIN | ADM_IN_DE_PIN | ADM_IN_NRE_PIN;
  gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
  gpio_init.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOA, &gpio_init); 
  //Set active low signal
  GPIO_SetBits(ADM_IN_NRE_PORT, ADM_IN_NRE_PIN);
  
  gpio_init.GPIO_Pin = ADM_OUT_DE_PIN | ADM_OUT_NRE_PIN;
  GPIO_Init(GPIOB, &gpio_init); 
  //Set active low signal
  GPIO_SetBits(ADM_OUT_NRE_PORT, ADM_OUT_NRE_PIN);
  
  //------------------------
  //---Init USART Modules---
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  //Configure USART3 (Transmitter)
  USART_InitTypeDef usart_init;
  usart_init.USART_BaudRate = 250000;
  usart_init.USART_WordLength = USART_WordLength_8b;
  usart_init.USART_StopBits = USART_StopBits_2;
  usart_init.USART_Parity = USART_Parity_No;
  usart_init.USART_Mode = USART_Mode_Tx;
  usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(USART_OUT, &usart_init);

  //Configure USART1 (Receiver)
  usart_init.USART_BaudRate = 250000;
  usart_init.USART_WordLength = USART_WordLength_8b;
  usart_init.USART_StopBits = USART_StopBits_2;
  usart_init.USART_Parity = USART_Parity_No;
  usart_init.USART_Mode = USART_Mode_Rx;
  usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(USART_IN, &usart_init);
  
  //Do not enable reveiver, because at first it is configured as EXTI GPIO to search for BREAK Signal
  //USART_Cmd(USART_IN, ENABLE);   
  USART_Cmd(USART_OUT, ENABLE);
  
  //Configure USART Pins -- again: USART_IN is at this moment EXTI
  gpio_init.GPIO_Pin = USART_OUT_TX_PIN;
  gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
  gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(USART_OUT_TX_PORT, &gpio_init);
  
  //--------------------------------------------------------------------
  //---Configure DMA for reading from and writing to USART Peripheral---
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  
  //USART_OUT DMA
  DMA_InitTypeDef dma_init;
  dma_init.DMA_PeripheralBaseAddr = (uint32_t)&USART_OUT->DR;
  dma_init.DMA_MemoryBaseAddr = (uint32_t)&dmx.buf;
  dma_init.DMA_DIR = DMA_DIR_PeripheralDST;                     //Memory -> USART
  dma_init.DMA_BufferSize = DMX_PACKET_LEN;
  dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;       //Peripheral Address (USARTx->DR) is always the same
  dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable;                //Memory is source --> shall be incremented at every read
  dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  dma_init.DMA_Mode = DMA_Mode_Normal;
  dma_init.DMA_Priority = DMA_Priority_Medium;
  dma_init.DMA_M2M = DMA_M2M_Disable;
  
  //cannels are related to periphal and cannot be chosen freely!
  DMA_Init(DMA1_Channel2, &dma_init);
  
  //USART_IN DMA
  dma_init.DMA_PeripheralBaseAddr = (uint32_t)&USART_IN->DR;
  dma_init.DMA_MemoryBaseAddr = (uint32_t)&dmx.buf;
  dma_init.DMA_DIR = DMA_DIR_PeripheralSRC;                     //USART -> Memory
  dma_init.DMA_BufferSize = DMX_PACKET_LEN;
  dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;       //Peripheral Address (USARTx->DR) is always the same
  dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable;                //Memory is source --> shall be incremented at every read
  dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  dma_init.DMA_Mode = DMA_Mode_Normal;
  dma_init.DMA_Priority = DMA_Priority_Medium;
  dma_init.DMA_M2M = DMA_M2M_Disable;
  
  DMA_Init(DMA1_Channel5, &dma_init);
  
  //Enable DMA Interrupts
  DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);
  DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);

  //After initializing in the Interrupt Module, Interrupts need to be initialized in the core as well
  NVIC_InitTypeDef nvic_init;
  nvic_init.NVIC_IRQChannel = DMA1_Channel2_IRQn;               //Interrupt channels cannot be initialzed by combining them with logic or
  nvic_init.NVIC_IRQChannelPreemptionPriority = 10;
  nvic_init.NVIC_IRQChannelSubPriority = 0;
  nvic_init.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_init);
  
  nvic_init.NVIC_IRQChannel = DMA1_Channel5_IRQn;
  NVIC_Init(&nvic_init);
  
  //Disable DMA interrupts, they'll be enabled once we need them
  USART_DMACmd(USART_OUT, USART_DMAReq_Tx, ENABLE);
  USART_DMACmd(USART_IN, USART_DMAReq_Rx, ENABLE);
  
  //----------------------------------------
  //---Configure Timers necessary for DMX---
  //Conveniently we use two timers: One for the frequency of DMX TX packets (TIM4)
  //and one to measure the break time to synchronize the receiver to a DMX packet (TIM3)
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

  //---init transmit timer---
  TIM_TimeBaseInitTypeDef timer_init;
  //ABP1 48 MHz --> 1 MHz
  timer_init.TIM_Prescaler = 48;
  timer_init.TIM_ClockDivision = TIM_CKD_DIV1;
  timer_init.TIM_CounterMode = TIM_CounterMode_Up;
  //Periode determins the time between packets
  //at timer overflow packet will be transmitted
  timer_init.TIM_Period = DMX_PERIOD_US;
  
  TIM_TimeBaseInit(TIM4, &timer_init);
  
  //output compare is used to create the break and mark after break (time after break until dmx packet starts)
  //OC1: Drive Ouput Pin low for break
  TIM_OCInitTypeDef oc_init;
  oc_init.TIM_OCMode = TIM_OCMode_Active;
  oc_init.TIM_Pulse = DMX_PERIOD_US - DMX_BREAK_TIME_US - DMX_MARK_AFTER_BREAK_US;
  TIM_OC1Init(TIM4, &oc_init);
  
  //OC2 drive pin high for mark after break
  oc_init.TIM_Pulse = DMX_PERIOD_US - DMX_MARK_AFTER_BREAK_US;
  TIM_OC2Init(TIM4, &oc_init);
  
  //---init receive timer---
  //ABP1 48 MHz --> 1 MHz
  timer_init.TIM_Prescaler = 48;
  timer_init.TIM_ClockDivision = TIM_CKD_DIV1;
  timer_init.TIM_CounterMode = TIM_CounterMode_Up;
  timer_init.TIM_Period = 88-1; //min break time for receiver is 88µs
  
  TIM_TimeBaseInit(TIM3, &timer_init);  
  
  //enable timer interrupts
  TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
  
  //Enable Interrupts in core
  //timer interrupt enabled in start routine
  nvic_init.NVIC_IRQChannel = TIM4_IRQn;
  nvic_init.NVIC_IRQChannelPreemptionPriority = 10;
  nvic_init.NVIC_IRQChannelSubPriority = 0;
  nvic_init.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_init);
  
  nvic_init.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_Init(&nvic_init);
  
  //--------------------------------------
  //---Configure EXTI necessary for DMX---
  //RX input pin is EXTI at startup after receiving a long enough break it will 
  //be reconfigured to a usart pin in the ISR of this EXTI 
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
}

void dmx_request_stop_transmit(){
  //Set STOP Flag. If this is done during a packet transmission, the packet will 
  //be transferred and then transmission is stopped
  if(dmx.transmitter.status == DMX_TRANSMIT_TRANSMITTING){
    dmx.transmitter.status = DMX_TRANSMIT_STOP_REQUESTED;
  }
}

void dmx_start_transmit(){
  TIM_Cmd(TIM4, ENABLE);
  TIM4_CLEAR_INTERRUPTS;
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
  TIM_ITConfig(TIM4, TIM_IT_CC1, ENABLE);
  TIM_ITConfig(TIM4, TIM_IT_CC2, ENABLE);
  dmx.transmitter.status = DMX_TRANSMIT_TRANSMITTING;
}

//enable exti for capturing falling edges on dmx line to capture break
void dmx_capture_frame(){
  EXTI->IMR |= EXTI_Line10;
}

void load_scene(uint8_t scene_id){
  uint16_t address = (DMX_PACKET_LEN-1) * scene_id;
  
  read_from_eeprom(address, (uint8_t*)&dmx.buf.data, DMX_PACKET_LEN-1);
}

void save_scene(uint8_t scene_id){
  uint16_t address = (DMX_PACKET_LEN-1) * scene_id;
  
  write_to_eeprom(address, (uint8_t*)&dmx.buf.data, DMX_PACKET_LEN - 1);
}