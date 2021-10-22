#define MODULE_MAIN
#include "project_header.h"
#include <string.h>

uint64_t last_led_flash;

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
  
  wait_ms(500);
  
  init_timers();
  init_usart();
  init_mem();
  init_dma();

  init_rx_exti();
  
  //init user buttons
  init_i2c_iox();
  init_iox_interrupt();
  init_menu_buttons();
  
  wait_ms(500);
  
  while(1){    
    lcd_repaint();
    fsm(); 
    
    if(millis > last_led_flash + 1000){
      last_led_flash=millis;
      
      if(LED_IS_OFF){
        LED_ON;
      }else{
        LED_OFF;
      }
    }
  }
}

void wait_ms(uint16_t ms){
  uint64_t end = millis + ms;
  while(end > millis);
}