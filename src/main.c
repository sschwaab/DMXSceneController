#define MODULE_MAIN
#include "project_header.h"

const uint8_t* str_bootup_1 = "Hochfahren      ";
const uint8_t* str_bootup_2 = "www.schwaab.cc  ";

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
  lcd_setcursor(0,0);
  lcd_putchars((uint8_t*)str_bootup_1, 16);
  lcd_setcursor(1,0);
  lcd_putchars((uint8_t*)str_bootup_2, 16);
  
  init_timers();
  init_usart();
  init_dma();

  init_rx_exti();
  
  while(1){    
    fsm();  
  }
}

void wait_ms(uint16_t ms){
  uint64_t end = millis + ms;
  while(end > millis);
}