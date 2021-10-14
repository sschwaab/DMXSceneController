#define MODULE_STATES
#include "project_header.h"
#include <string.h>

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
        GPIO_ResetBits(RELAY_SEND_PORT, RELAY_SEND_PIN);
        GPIO_ResetBits(ADM_OUT_DE_PORT, ADM_OUT_DE_PIN);
        
        wait_ms(10);
        
        //update LCD
        strcpy(lcd.line1, str_passtrough_1);
        strcpy(lcd.line2, str_null);
        lcd.repaint = LCD_REPAINT;
        
        state = ST_PASSTHROUGH;

      }else{
        //Load Scene from Memory and set state accordingly        
        load_scene(scene_id);
        dmx_start_transmit();
        state=ST_DMX_MASTER;
        
        GPIO_SetBits(RELAY_SEND_PORT, RELAY_SEND_PIN);
        GPIO_SetBits(ADM_OUT_DE_PORT, ADM_OUT_DE_PIN);
        wait_ms(10);
        
        //update LCD
        char szene_id_ascii = 49U + scene_id;
        strcpy(lcd.line1, str_recall_1);
        strcpy(lcd.line2, str_recall_2);
        strcat(lcd.line2, &szene_id_ascii); 
        lcd.repaint = LCD_REPAINT;
      }
      //Button press is processed -> reset it
      recall_buttons = 0x00;
    }
    
    break;
    
  case ST_DMX_MASTER:  
    //Recall was requested
    //Stop transmit, and wait until stopped before loading scene
    
    if(recall_buttons != 0x00 || menu_buttons & MENU_BUTTON_RECORD || dmx.transmitter_status == DMX_TRANSMIT_STOP_REQUESTED){
      dmx_request_stop_transmit();
      
      if(dmx.transmitter_status == DMX_TRANSMIT_STOPPED){
        GPIO_ResetBits(RELAY_SEND_PORT, RELAY_SEND_PIN);
        GPIO_ResetBits(ADM_OUT_DE_PORT, ADM_OUT_DE_PIN);
        if(recall_buttons != 0x00){
          state=ST_LOAD_SCENE;
        }else if(menu_buttons & MENU_BUTTON_RECORD){
          state=ST_RECORD_PRE;
          menu_buttons = 0x00;
          dmx.recorder.scene_id = DMX_SCENE_ID_INVALID;
        }
      }
    }
    break;
  
  case ST_PASSTHROUGH:
    if(recall_buttons == 0x80){
      recall_buttons = 0x00;
    }else if(recall_buttons != 0x00){
      state=ST_LOAD_SCENE;
    }else if(menu_buttons & MENU_BUTTON_RECORD){
      state = ST_RECORD_PRE;
      menu_buttons = 0x00;
      dmx.recorder.scene_id = DMX_SCENE_ID_INVALID;
    }
    break;
    
  case ST_RECORD_PRE:
    
    //TODO TIMEOUT
    
    if(menu_buttons & MENU_BUTTON_UP || menu_buttons & MENU_BUTTON_DOWN || 
       dmx.recorder.scene_id == DMX_SCENE_ID_INVALID){
      
      if(dmx.recorder.scene_id == DMX_SCENE_ID_INVALID){      
         dmx.recorder.scene_id = 0;   
      }
      
      if(menu_buttons & MENU_BUTTON_UP){
        if(dmx.recorder.scene_id < 6){
          dmx.recorder.scene_id++;
        }
        menu_buttons &= ~ MENU_BUTTON_UP;
      }
    
      if(menu_buttons & MENU_BUTTON_DOWN){
        if(dmx.recorder.scene_id>0){
          dmx.recorder.scene_id--;
        }
        menu_buttons &= ~ MENU_BUTTON_DOWN;
      }
      
      //update LCD
      char szene_id_ascii_save = 49U + dmx.recorder.scene_id;
      strcpy(lcd.line1, str_record_1);
      strcpy(lcd.line2, str_record_2);
          
      strcat(lcd.line1, &szene_id_ascii_save); 
      
      lcd.repaint = LCD_REPAINT;
    }else if(menu_buttons & MENU_BUTTON_RECORD){
      GPIO_SetBits(RELAY_RECEIVE_PORT, RELAY_RECEIVE_PIN);
      GPIO_ResetBits(ADM_IN_NRE_PORT, ADM_IN_NRE_PIN);
      wait_ms(20);
      state = ST_RECORD;
      dmx.recorder.recorder_status = DMX_RECORD_SearchFrame1;
      menu_buttons = 0;
    }else if(menu_buttons & MENU_BUTTON_EXIT){
      //load scene 0
      recall_buttons = 0x01;
      menu_buttons = 0;
      state=ST_LOAD_SCENE;
    }
    break;
    
  case ST_RECORD:
    //switch substates
    switch(dmx.recorder.recorder_status){
    case DMX_RECORD_SearchFrame1:
      dmx.recorder.frame_found = DMX_RECORDER_NO_FRAME_FOUND;
      dmx.recorder.end_time = millis + DMX_TIMEOUT_MILLIS; 
      
      strcpy(lcd.line1, str_record_searchframe_1);
      strcpy(lcd.line2, str_null);
      lcd.repaint = LCD_REPAINT;
      
      dmx_capture_frame();
      
      dmx.recorder.recorder_status = DMX_RECORD_SearchFrame2;
      
      break;
      
    case DMX_RECORD_SearchFrame2:
      if(millis > dmx.recorder.end_time){
        strcpy(lcd.line1, str_record_timeout_1);
        strcpy(lcd.line2, str_record_timeout_2);
        lcd.repaint = LCD_REPAINT;
        dmx.recorder.recorder_status = DMX_RECORD_TIMEOUT;
      }else{
        if(dmx.recorder.frame_found == DMX_RECORDER_FRAME_FOUND){
          strcpy(lcd.line1, str_record_foundframe_1);
          strcpy(lcd.line2, str_record_foundframe_2);
          lcd.repaint = LCD_REPAINT;
         
          dmx.recorder.recorder_status = DMX_RECORD_SaveFrame;
        }
      }
      break;
      
    case DMX_RECORD_SaveFrame:
      wait_ms(1000);
      //TODO Save to EEprom
      dmx.recorder.recorder_status = DMX_RECORD_Done;
      break;
      
    case DMX_RECORD_Done:
      strcpy(lcd.line1, str_record_savedframe_1);
      strcpy(lcd.line2, str_record_savedframe_2);
      lcd.repaint = LCD_REPAINT;
  
      GPIO_SetBits(RELAY_RECEIVE_PORT, RELAY_RECEIVE_PIN);
      GPIO_ResetBits(ADM_IN_NRE_PORT, ADM_IN_NRE_PIN);
      
      wait_ms(20);
      
      state = ST_RECORD_POST;
      
      break;
      
    case DMX_RECORD_TIMEOUT:
      if(menu_buttons & MENU_BUTTON_EXIT){
        recall_buttons = 0x01;
        state = ST_LOAD_SCENE;
        menu_buttons = 0;
      }
    }
    break;
    
    case ST_RECORD_POST:
      if(menu_buttons & MENU_BUTTON_EXIT){
        recall_buttons = 0x01 << dmx.recorder.scene_id;
        state = ST_LOAD_SCENE;
        menu_buttons = 0;
      }
    break;
  
  default:
    break;
  }
}