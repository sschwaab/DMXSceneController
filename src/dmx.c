#define MODULE_DMX
#include "project_header.h"

void dmx_request_stop_transmit(){
  if(dmx.transmitter_status == DMX_TRANSMIT_TRANSMITTING){
    dmx.transmitter_status = DMX_TRANSMIT_STOP_REQUESTED;
  }
}

void dmx_start_transmit(){
  TIM_Cmd(TIM4, ENABLE);
  TIM4_CLEAR_INTERRUPTS;
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
  TIM_ITConfig(TIM4, TIM_IT_CC1, ENABLE);
  TIM_ITConfig(TIM4, TIM_IT_CC2, ENABLE);
  dmx.transmitter_status = DMX_TRANSMIT_TRANSMITTING;
}

void dmx_capture_frame(){
  EXTI->IMR |= EXTI_Line10;
}

void load_scene(uint8_t scene_id){
  uint16_t address = (DMX_PACKET_LEN-1) * scene_id;
  
  read_from_eeprom(address, (uint8_t*)&dmx.buf.data, DMX_PACKET_LEN-1);
}

void save_scene(uint8_t scene_id){
  uint16_t address = (DMX_PACKET_LEN-1) * scene_id;
  
  write_to_eeprom(address, (uint8_t*)&dmx.buf.data, DMX_PACKET_LEN - 1);
}