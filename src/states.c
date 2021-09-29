#define MODULE_STATES
#include "project_header.h"

typedef enum{
  ST_BOOT,
  ST_LOAD_SCENE,
  ST_DMX_MASTER,
  ST_PASSTHROUGH,
  ST_RECORD_PRE,
  ST_RECORD,
  ST_RECORD_POST
}tStates;

tStates state;

void fsm(){
  switch(state){
  case ST_BOOT:
    //Fallthrough to load scene state
    recall_buttons = 0x01;
    state = ST_LOAD_SCENE;
      
  case ST_LOAD_SCENE:
    if(recall_buttons != 0x00){
      //Convert button to number
      uint8_t scene_id;
      for(scene_id=0; scene_id<8; scene_id++){
        if(recall_buttons & (0x01 << scene_id)){
          break;
        }
      }
      
      //Button 7 is passthrough
      if(scene_id==7){
        state = ST_PASSTHROUGH;
      }else{
        load_scene(scene_id);
        dmx_start_transmit();
        state=ST_DMX_MASTER;
      }
      
      recall_buttons = 0x00;
    }
    
    break;
    
  case ST_DMX_MASTER:  
    //Recall was requested
    //Stop transmit, and wait until stopped before loading scene
        
    if(dmx.transmitter_status == DMX_TRANSMIT_STOPPED){
      state=ST_LOAD_SCENE;
    }
    
    if(recall_buttons != 0x00){
      dmx_request_stop_transmit();
    }

    break;
  
  case ST_PASSTHROUGH:
    if(recall_buttons == 0x80){
      recall_buttons = 0x00;
    }else if(recall_buttons != 0x00){
      //TODO: Switch Relays
      state=ST_LOAD_SCENE;
    }
    break;
  
  case ST_RECORD_PRE:
    
    break;
    
  case ST_RECORD:
    
    break;
    
  case ST_RECORD_POST:
    
    break;
    
  default:
    break;
  }
}