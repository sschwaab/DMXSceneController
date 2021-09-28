#ifndef IO_H
#define IO_H

#define LED_PORT GPIOC
#define LED_PIN GPIO_Pin_13

#define DISP_DB4_PORT GPIOA
#define DISP_DB4_PIN GPIO_Pin_0

#define DISP_DB5_PORT GPIOA
#define DISP_DB5_PIN GPIO_Pin_1

#define DISP_DB6_PORT GPIOA
#define DISP_DB6_PIN GPIO_Pin_2

#define DISP_DB7_PORT GPIOA
#define DISP_DB7_PIN GPIO_Pin_3

#define DISP_E_PORT GPIOA
#define DISP_E_PIN GPIO_Pin_4

#define DISP_RS_PORT GPIOA
#define DISP_RS_PIN GPIO_Pin_5

#define USART_IN USART1

#define USART_OUT USART3
#define USART_OUT_TX_PORT GPIOB
#define USART_OUT_TX_PIN GPIO_Pin_10
#define USART_OUT_TX_CNF_MASK 0x00000C00;
#define USART_OUT_TX_CNF_POS 8

#define TIM4_CLEAR_INTERRUPTS TIM_ClearITPendingBit(TIM4, TIM_IT_Update | TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4 | TIM_IT_COM | TIM_IT_Trigger | TIM_IT_Break)


#endif