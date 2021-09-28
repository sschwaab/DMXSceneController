#define MODULE_MAIN
#include "project_header.h"

uint8_t lcd_buf[] = "Hallo Welt      ";

uint8_t main(){
  
#ifdef DEBUG
  uint16_t i_dummydmxdata;
  for(i_dummydmxdata = 0; i_dummydmxdata<512; i_dummydmxdata++){
    dmx.buf.data[i_dummydmxdata] = i_dummydmxdata%256;   
  }
  
  
#endif
  
  init_core();
  init_clock();
  init_io();
  init_lcd();
  init_timers();
  
  init_usart();
  init_dma();

  dmx_start_transmit();
  while(1){
  /*lcd_setcursor(0,3);
  lcd_putchars(lcd_buf, 10);
  
  wait_ms(1000);
  lcd_clear();
  
  lcd_setcursor(1,3);
  lcd_putchars(lcd_buf, 10);
  
  wait_ms(1000);
  lcd_clear();
  */
  }
}

void wait_ms(uint16_t ms){
  uint64_t end = millis + ms;
  while(end > millis);
}