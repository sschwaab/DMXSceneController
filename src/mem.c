#define MODULE_MEM
#include "project_header.h"

void init_mem(){
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 
  
  //Configure CS as output
  GPIO_InitTypeDef gpio_init;
  
  gpio_init.GPIO_Pin = MEM_N_CS_PIN;
  gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
  gpio_init.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOB, &gpio_init);   

  GPIO_SetBits(MEM_N_CS_PORT, MEM_N_CS_PIN);
  
  //Configure SPI Module
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  
  SPI_InitTypeDef spi_init;
  
  spi_init.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  spi_init.SPI_Mode = SPI_Mode_Master;
  spi_init.SPI_DataSize = SPI_DataSize_8b;
  spi_init.SPI_CPOL = SPI_CPOL_High; 
  spi_init.SPI_CPHA = SPI_CPHA_2Edge;
  spi_init.SPI_NSS = SPI_NSS_Soft;
  spi_init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
  spi_init.SPI_FirstBit = SPI_FirstBit_MSB;
  
  SPI_Init(SPI1, &spi_init);
  
  //Configure CLK, MOSI, MISO Pins as Alternate function and remap them
  gpio_init.GPIO_Pin = SPI_CLK_PIN | SPI_MOSI_PIN | SPI_MISO_PIN;
  gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
  gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &gpio_init); 
  
  GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
  
  SPI_Cmd(SPI1, ENABLE);
  (void)SPI1->DR;//TODO: Check if necessary
}

void write_to_eeprom(uint16_t address, uint8_t* buf, uint16_t len){
  uint16_t bytes_sent = 0;
  
  //USE SPI in Blocking Mode. No Need for Intrerrups
  
  while(bytes_sent < len){
    //1. Unlock EEPROM
    GPIO_ResetBits(MEM_N_CS_PORT, MEM_N_CS_PIN);
    SPI_I2S_SendData(SPI1, 0x06);
    while(SPI1->SR & SPI_SR_BSY);
    GPIO_SetBits(MEM_N_CS_PORT, MEM_N_CS_PIN);
    (void)SPI1->DR;
    //2. Send Write Command
    GPIO_ResetBits(MEM_N_CS_PORT, MEM_N_CS_PIN);
    SPI_I2S_SendData(SPI1, 0x02);
    while(SPI1->SR & SPI_SR_BSY);
    
    //3. Send Address
    uint8_t addr_high = (address & 0xFF00) >> 8;
    uint8_t addr_low = address & 0x00FF;
    SPI_I2S_SendData(SPI1, addr_high);
    while(SPI1->SR & SPI_SR_BSY);
    SPI_I2S_SendData(SPI1, addr_low);
    while(SPI1->SR & SPI_SR_BSY);
    
    //write first byte
    SPI_I2S_SendData(SPI1, buf[bytes_sent]);
    while(SPI1->SR & SPI_SR_BSY);
    bytes_sent++;
    address++;
    
    while(bytes_sent < len && address % 128 != 0){
      SPI_I2S_SendData(SPI1, buf[bytes_sent]);
      address++;
      bytes_sent++;
      while(SPI1->SR & SPI_SR_BSY);
    }
   wait_ms(2); 
   GPIO_SetBits(MEM_N_CS_PORT, MEM_N_CS_PIN);
  }
}

void read_from_eeprom(uint16_t address, uint8_t* buf, uint16_t len){
  GPIO_ResetBits(MEM_N_CS_PORT, MEM_N_CS_PIN);
  //1. Send Read Command
  SPI_I2S_SendData(SPI1, 0x03);
  while(SPI1->SR & SPI_SR_BSY);
  
   //3. Send Address
  uint8_t addr_high = (address & 0xFF00) >> 8;
  uint8_t addr_low = address & 0x00FF;
  
  SPI_I2S_SendData(SPI1, addr_high);
  while(SPI1->SR & SPI_SR_BSY);
  SPI_I2S_SendData(SPI1, addr_low);
  while(SPI1->SR & SPI_SR_BSY);
  
  //empty DR
  (void)SPI1->DR;
 
  
  //Read Data
  while(len--){
    SPI1->DR = 0x00;
    while(SPI1->SR & SPI_SR_BSY);
    *buf = SPI1->DR;
    buf++;
  }
  
  GPIO_SetBits(MEM_N_CS_PORT, MEM_N_CS_PIN);
}