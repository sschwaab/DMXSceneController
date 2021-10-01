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
  for(int i = 0; i<512; i++){
    dmx.buf.data[i] = scene_id<<4 | scene_id;
  }
}
