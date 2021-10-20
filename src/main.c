#define MODULE_MAIN
#include "project_header.h"
#include <string.h>


uint8_t main(){
  init_core();
  init_clock();
  init_io();
  
  init_lcd();
  
  GPIO_SetBits(DISP_BL_PORT, DISP_BL_PIN);
  
  strcpy((char*)lcd.line1, (const char*)str_bootup_1);
  strcpy((char*)lcd.line2, (const char*)str_bootup_2);
  lcd.repaint = LCD_REPAINT;
  lcd_repaint();
  
  init_timers();
  init_usart();
  init_mem();
  init_dma();

  init_rx_exti();
  init_i2c_iox();
  init_iox_interrupt();
  
  wait_ms(500);
  
  while(1){    
    lcd_repaint();
    fsm();  
  }
}

void wait_ms(uint16_t ms){
  uint64_t end = millis + ms;
  while(end > millis);
}