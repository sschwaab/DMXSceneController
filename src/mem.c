#define MODULE_MEM
#include "project_header.h"

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