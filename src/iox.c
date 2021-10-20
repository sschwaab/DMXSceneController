#define MODULE_IOX
#include "project_header.h"

void init_i2c_iox(){
  GPIO_InitTypeDef gpio_init;
  I2C_InitTypeDef i2c_init;

  // Configure I2C1 Pins
  gpio_init.GPIO_Pin = I2C_SCK_PIN | I2C_SDA_PIN;
  gpio_init.GPIO_Mode = GPIO_Mode_AF_OD;
  gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(I2C_PORT, &gpio_init);

  GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_TIM4, ENABLE);
  
  GPIO_SetBits(I2C_PORT, I2C_SCK_PIN);
  GPIO_SetBits(I2C_PORT, I2C_SDA_PIN);
  
  // clock enable
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  
  I2C_SoftwareResetCmd(I2C1, ENABLE);
  I2C_SoftwareResetCmd(I2C1, DISABLE);

  i2c_init.I2C_Mode = I2C_Mode_I2C;
  i2c_init.I2C_DutyCycle = I2C_DutyCycle_2;
  i2c_init.I2C_OwnAddress1 = 0x00;
  i2c_init.I2C_Ack = I2C_Ack_Enable;
  i2c_init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  i2c_init.I2C_ClockSpeed = 100000; //100kHz
  
  I2C_Init(I2C1, &i2c_init);
  
  //I2C_SoftwareResetCmd(I2C1, ENABLE);
  //I2C_SoftwareResetCmd(I2C1, DISABLE); 
  
  uint8_t i2c_config[3];

  i2c_config[0] = 0x06; // config port 0 & 1
  i2c_config[1] = 0xFF;   // Port0: all pins outputs
  i2c_config[2] = 0x00;   // Port1: all pins outputs
  i2c_transmit_data(I2C1, 0x40, &i2c_config[0], 3);
}

void iox_set_led(uint8_t led){
  uint8_t i2c_config[2];

  i2c_config[0] = 0x03; // command: output register
  i2c_config[1] = led;
  
  i2c_transmit_data(I2C1, 0x40, &i2c_config[0], 3);
}

uint8_t iox_read_input(I2C_TypeDef *I2Cx, uint8_t i2c_addr){
  EXTI->IMR &= ~EXTI_IMR_MR7;
  
  uint8_t buf = 0x00;
  
  i2c_transmit_data(I2C1, 0x40, &buf, 1);
  
  // disable acknowledge of received data
  // nack also generates stop condition after last byte received
  // see reference manual for more info
  //I2C_AcknowledgeConfig(I2Cx, DISABLE);
  //I2C_GenerateSTOP(I2Cx, ENABLE);
  
  while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
  I2C_GenerateSTART(I2Cx, ENABLE);
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)); // check if MASTER MODE selected for STM32

   // send Address 7-Bit mode
  I2C_Send7bitAddress(I2Cx, i2c_addr, I2C_Direction_Receiver);
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
  
  I2C_AcknowledgeConfig(I2C1,ENABLE);
  // wait until one byte has been received
  while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
  // read data from I2C data register and return data byte
  uint8_t data = I2C_ReceiveData(I2Cx);
  I2C_AcknowledgeConfig(I2C1, DISABLE);
  I2C_GenerateSTOP(I2C1, ENABLE);
  EXTI->IMR |= EXTI_IMR_MR7;
  
  
  return data;  
}

void i2c_transmit_data(I2C_TypeDef *I2Cx, uint8_t i2c_addr, uint8_t *buf, uint8_t size) {
  // wait for ready
  while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));

  // generate start condition by master
  I2C_GenerateSTART(I2Cx, ENABLE);
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)); // check if MASTER MODE selected for STM32

  // send Address 7-Bit mode
  I2C_Send7bitAddress(I2Cx, i2c_addr, I2C_Direction_Transmitter);
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  // send data
  while (size--) {
    I2C_SendData(I2Cx, *buf++); // send register to write address in slave device
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));  // check if register to write address transmitted
  }
  // stop event
  I2C_GenerateSTOP(I2Cx, ENABLE);
}

void init_iox_interrupt(){
  //Configure ExtInitLine  
  GPIO_InitTypeDef gpio_init;
  
  gpio_init.GPIO_Pin = IOX_INTERRUPT_PIN;
  gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
  gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  
  GPIO_Init(IOX_INTERRUPT_PORT, &gpio_init); 
  
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource7);
  
  EXTI_InitTypeDef exti_init;
  exti_init.EXTI_Line = EXTI_Line7;
  exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
  exti_init.EXTI_Trigger = EXTI_Trigger_Falling;
  exti_init.EXTI_LineCmd = ENABLE;
  
  EXTI_Init(&exti_init); 
  
  NVIC_InitTypeDef nvic_init;
  nvic_init.NVIC_IRQChannel = EXTI9_5_IRQn;
  nvic_init.NVIC_IRQChannelPreemptionPriority = 9;
  nvic_init.NVIC_IRQChannelSubPriority = 0;
  nvic_init.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_init);
}