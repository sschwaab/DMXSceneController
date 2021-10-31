/**
  ******************************************************************************
  * @file    NVIC/IRQ_Mask/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "project_header.h"

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

void SVC_Handler(void)
{
  
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
  millis++;
}

void EXTI0_IRQHandler(void)
{
 
} 

void TIM2_IRQHandler(void)
{
  
}

//Timer 3 is record timer to measure break signal, if this timer overflows, a valid break condition was received
//(valid  = long enough)
void TIM3_IRQHandler(void)
{
  //Disable Timer and clear flags
  TIM3->SR &= 0;                    //Clear Flags
  TIM3->DIER &= ~TIM_IT_Update;     //Disable Interrupt
  TIM3->CR1 &= ~TIM_CR1_CEN;        //Stop Counter
  
  EXTI->IMR &= ~EXTI_Line10;    //Disable Break Search EXTI

  //Enable USART peripheral, since pin is only input, it does not need to be reconfigured to alternate function
  USART_IN->CR1 |= USART_CR1_UE;
   //And Start DMA to sample the data
  DMA1_Channel5->CNDTR = DMX_PACKET_LEN;
  DMA_Cmd(DMA1_Channel5, ENABLE);
}


//Timer 4 is to generate the break signal for TX and determines the periode of dmx packets
void TIM4_IRQHandler(void)
{
  
  if((TIM4->SR & TIM_IT_Update) && (TIM4->DIER & TIM_IT_Update)){
    //send data
    USART_OUT_TX_PORT->CRH |= 0x08 << USART_OUT_TX_CNF_POS;
    DMA1_Channel2->CNDTR = DMX_PACKET_LEN;
    DMA_Cmd(DMA1_Channel2, ENABLE);
  }else if((TIM4->SR & TIM_IT_CC1) && (TIM4->DIER & TIM_IT_CC1)){
    //Drive Low to generate break signal
    USART_OUT_TX_PORT->BSRR |= USART_OUT_TX_PIN << 16;
    //Set Output as GPIO
    USART_OUT_TX_PORT->CRH &= ~USART_OUT_TX_CNF_MASK;
  }else if((TIM4->SR & TIM_IT_CC2) && (TIM4->DIER & TIM_IT_CC2)){
    //drive high to generate mark (high time after break)
    USART_OUT_TX_PORT->BSRR |= USART_OUT_TX_PIN;
  }
  
  TIM4_CLEAR_INTERRUPTS;
}

//Transmit DMA
void DMA1_Channel2_IRQHandler(){
  if(DMA_GetITStatus(DMA1_IT_TC2)){
    DMA_ClearITPendingBit(DMA1_IT_GL2);
    //transfer complete --> Disable DMA but make it ready for next transmission
    //if stop is requested initiate stop
    if(dmx.transmitter.status == DMX_TRANSMIT_STOP_REQUESTED || 
      dmx.transmitter.status == DMX_TRANSMIT_STOPPED){
      DMA_Cmd(DMA1_Channel2, DISABLE);
      TIM_Cmd(TIM4, DISABLE);
      TIM4_CLEAR_INTERRUPTS;

      dmx.transmitter.status = DMX_TRANSMIT_STOPPED;     
    }else{
      DMA_Cmd(DMA1_Channel2, DISABLE);
      DMA1_Channel2->CNDTR = DMX_PACKET_LEN;
    }
  }
}

void DMA1_Channel5_IRQHandler(){
  //Full packet was sampled --> tell it to the state machine
  if(DMA_GetITStatus(DMA1_IT_TC5)){
    DMA_ClearITPendingBit(DMA1_IT_GL5);
    DMA_Cmd(DMA1_Channel5, DISABLE);
    USART_IN->CR1 &= ~USART_CR1_UE;

    dmx.recorder.frame_found = DMX_RECORDER_FRAME_FOUND; 
  }
}

void EXTI15_10_IRQHandler(){
  //GET BREAK from DMX
  if((EXTI->PR & EXTI_Line10) && (EXTI->IMR & EXTI_Line10)){
    EXTI->PR = EXTI_Line10;
    
    if(GPIOA->IDR & USART_IN_RX_PIN){
      //Pin is high --> Stop Counter
      TIM3->DIER &= ~TIM_IT_Update;      //Disable Interrupt
      TIM3->CR1 &= ~TIM_CR1_CEN;
    }else{
      //Pin is low --> Start Counter to measure break length
      TIM3->CNT = 0;
      TIM3->CR1 |= TIM_CR1_CEN;         //Start Counter
      TIM3->SR &= 0;                    //Clear Flags
      TIM3->DIER |= TIM_IT_Update;      //Enable Interrupt
    }
  }
  
  //Menu buttons are on the same interrupt
  //handle them and clear interrupt flag
  if((EXTI->PR & EXTI_Line12) && (EXTI->IMR & EXTI_Line12)){
    EXTI->PR = EXTI_Line12;
    menu_buttons |= MENU_BUTTON_RECORD;
  }
  
  if((EXTI->PR & EXTI_Line13) && (EXTI->IMR & EXTI_Line13)){
    EXTI->PR = EXTI_Line13;
    menu_buttons |= MENU_BUTTON_DOWN;
  }
  
  if((EXTI->PR & EXTI_Line14) && (EXTI->IMR & EXTI_Line14)){
    EXTI->PR = EXTI_Line14;
    menu_buttons |= MENU_BUTTON_UP;
  }
  
  if((EXTI->PR & EXTI_Line15) && (EXTI->IMR & EXTI_Line15)){
    EXTI->PR = EXTI_Line15;
    menu_buttons |= MENU_BUTTON_EXIT;
  }
}

uint8_t user_buttons_isr;

//this is the not interrupt pin from the ioexpander
void EXTI9_5_IRQHandler(){
  //jung button made problems when reading in isr
  //isr only sets flag and i2c is read in state machine
  //seems to be fast enough
  EXTI->PR = EXTI_Line7;
  iox_isr_flag = 0x01;
}