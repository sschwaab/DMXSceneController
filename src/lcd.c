#define MODULE_LCD
#include "project_header.h"

void lcd_write4bit(uint8_t nibble){
  if(nibble & 0x01){
    GPIO_SetBits(LCD_PORT, LCD_DB4);
  }else{
    GPIO_ResetBits(LCD_PORT, LCD_DB4);
  }
  
  if(nibble & 0x02){
    GPIO_SetBits(LCD_PORT, LCD_DB5);
  }else{
    GPIO_ResetBits(LCD_PORT, LCD_DB5);
  }
  
  if(nibble & 0x04){
    GPIO_SetBits(LCD_PORT, LCD_DB6);
  }else{
    GPIO_ResetBits(LCD_PORT, LCD_DB6);
  }
  
  if(nibble & 0x08){
    GPIO_SetBits(LCD_PORT, LCD_DB7);
  }else{
    GPIO_ResetBits(LCD_PORT, LCD_DB7);
  }
  
  //TODO: Make sure that this pulse is at least 450ns long
  GPIO_SetBits(LCD_PORT, LCD_EN);
  uint8_t i;
  for(i=0; i<10; i++);
  GPIO_ResetBits(LCD_PORT, LCD_EN);
}

void lcd_write(uint8_t byte){
  //write high nibble first see fig. 7
  uint8_t nibble = byte >> 4;
  lcd_write4bit(nibble);
   
  //write low nibble
  nibble = byte & 0x0F;
  lcd_write4bit(nibble);
}

void init_lcd(){
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
  GPIO_InitTypeDef gpio_init;
  gpio_init.GPIO_Pin = DISP_DB4_PIN | DISP_DB5_PIN| DISP_DB6_PIN| DISP_DB7_PIN;
  gpio_init.GPIO_Pin |= DISP_E_PIN | DISP_RS_PIN | DISP_BL_PIN;  
  
  gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
  gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
   
  GPIO_Init(GPIOA, &gpio_init); 
  
  //make sure to wait 15ms after power up
  wait_ms(15);
  GPIO_ResetBits(LCD_PORT, LCD_RS);
  
  lcd_write4bit(0x03);
  wait_ms(5);
  lcd_write4bit(0x03);
  wait_ms(1);
  lcd_write4bit(0x03);
  wait_ms(1);
  //Display is now initialized
  
  lcd_write4bit(0x02);      //turn on 4bit mode
  wait_ms(1);
  
  lcd_write(0x28);          //00101000 - 4bit, 2 lines, 5x8 font
  wait_ms(1);
  
  lcd_write(0x0C); //0x0c         //00001100 - display on cursor and blinking off
  wait_ms(1);
  
  lcd_write(0x01);          //Clear Display
  wait_ms(1);
  
  lcd_write(0x06);          //Set entry point
  wait_ms(1); 
}

void lcd_setcursor(uint8_t row, uint8_t col){
  GPIO_ResetBits(LCD_PORT, LCD_RS);
  row = row & 0x01;
  row = row << 6;
  
  col = col & 0x0F;
  
  lcd_write(0x80 | col | row);
  LCD_WAIT
  LCD_WAIT
}

void lcd_putchars(uint8_t* chr, uint8_t len){
  GPIO_SetBits(LCD_PORT, LCD_RS);
  uint8_t i;
  for(i = 0; i<len && *chr!=0x00; i++){
    if(*chr > 31 && *chr < 126){
      lcd_write(*chr);
    }else{
      lcd_write(' ');
    }
    chr++;
  
    LCD_WAIT
  }
}

void lcd_clear(){
  GPIO_ResetBits(LCD_PORT, LCD_RS);
  
  lcd_write(0x01);
  wait_ms(3);
}

void lcd_repaint(){
  if(lcd.repaint == LCD_REPAINT){
    lcd_clear();
    lcd_setcursor(0,0);
    lcd_putchars((uint8_t*)lcd.line1,16);
    lcd_setcursor(1,0);
    lcd_putchars((uint8_t*)lcd.line2,16);
    lcd.repaint = LCD_NO_REPAINT;
  }
}