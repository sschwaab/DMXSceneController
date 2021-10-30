#ifndef IO_H
#define IO_H

#define LED_PORT GPIOC
#define LED_PIN GPIO_Pin_13

//******************************************************//
//DISPLAY
//******************************************************//
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

#define DISP_BL_PORT GPIOA
#define DISP_BL_PIN GPIO_Pin_8

//******************************************************//
//USART
//******************************************************//
#define USART_IN USART1
#define USART_IN_RX_PORT GPIOA
#define USART_IN_RX_PIN GPIO_Pin_10
#define USART_IN_RX_CNF_MASK 0x00000C00;
#define USART_IN_RX_CNF_POS 8

#define USART_OUT USART3
#define USART_OUT_TX_PORT GPIOB
#define USART_OUT_TX_PIN GPIO_Pin_10
#define USART_OUT_TX_CNF_MASK 0x00000C00;
#define USART_OUT_TX_CNF_POS 8

#define ADM_IN_NRE_PORT GPIOA
#define ADM_IN_NRE_PIN GPIO_Pin_11
#define ADM_IN_DE_PORT GPIOA
#define ADM_IN_DE_PIN GPIO_Pin_12

#define ADM_OUT_NRE_PORT GPIOB
#define ADM_OUT_NRE_PIN GPIO_Pin_0
#define ADM_OUT_DE_PORT GPIOB
#define ADM_OUT_DE_PIN GPIO_Pin_1

//******************************************************//
//RELAYS
//******************************************************//
#define RELAY_SEND_PORT GPIOA
#define RELAY_SEND_PIN GPIO_Pin_6

#define RELAY_RECEIVE_PORT GPIOA
#define RELAY_RECEIVE_PIN GPIO_Pin_7

//******************************************************//
//MEM
//******************************************************//
#define MEM_N_CS_PORT GPIOB
#define MEM_N_CS_PIN GPIO_Pin_6

#define SPI_PORT GPIOB
#define SPI_CLK_PIN GPIO_Pin_3
#define SPI_MOSI_PIN GPIO_Pin_5
#define SPI_MISO_PIN GPIO_Pin_4

//******************************************************//
//IO Expander
//******************************************************//
#define I2C_PORT GPIOB
#define I2C_SDA_PIN GPIO_Pin_9
#define I2C_SCK_PIN GPIO_Pin_8

#define IOX_INTERRUPT_PORT GPIOB
#define IOX_INTERRUPT_PIN GPIO_Pin_7

//******************************************************//
//Menu Buttons
//******************************************************//
#define MENU_BUTTONS_PORT GPIOB
#define MENU_BUTTON_RECORD_PIN GPIO_Pin_12
#define MENU_BUTTON_DOWN_PIN GPIO_Pin_13
#define MENU_BUTTON_UP_PIN GPIO_Pin_14
#define MENU_BUTTON_EXIT_PIN GPIO_Pin_15

//******************************************************//
//Onbard LED
//******************************************************//
#define LED_PORT GPIOC
#define LED_PIN GPIO_Pin_13

//******************************************************//
//User Button Power LED
//******************************************************//  
#define LED_USER_PORT GPIOA
#define LED_USER_PIN GPIO_Pin_15

#ifdef MODULE_IO
  void init_io();
#else
  extern void init_io();
#endif  
#endif