#define MODULE_STATES
#include "project_header.h"
#include <string.h>

tStates state;

void fsm(){
  switch(state){
  case ST_BOOT:
    //scene after bootup
    dmx.scene_id = 0;
    state = ST_LOAD_SCENE;
    break;
    
  case ST_PROCESS_FRONT_BUTTONS:
    //Convert button to number
    if(recall_buttons != 0){
      for(dmx.scene_id=0; dmx.scene_id<8; dmx.scene_id++){
        if(recall_buttons & (0x01 << dmx.scene_id)){
          break;
        }
      }
      
      //load scene in next iteration
      state = ST_LOAD_SCENE;
      recall_buttons = 0x00;
    }else{
      //keep current scene
      state = ST_LOAD_SCENE;
    }
    break;

    
  case ST_LOAD_SCENE:     
    //scene 7 is passthrough
    if(dmx.scene_id==7){
      GPIO_ResetBits(RELAY_SEND_PORT, RELAY_SEND_PIN);
      GPIO_ResetBits(ADM_OUT_DE_PORT, ADM_OUT_DE_PIN);
      
      //relay turn on time
      wait_ms(10);
        
      //update LCD
      strcpy(lcd.line1, str_passtrough_1);
      strcpy(lcd.line2, str_null);
      lcd.repaint = LCD_REPAINT;
               
      state = ST_PASSTHROUGH;

    }else{
      //Load Scene from Memory and set state accordingly        
      load_scene(dmx.scene_id);
      dmx_start_transmit();
    
      state=ST_DMX_MASTER;
        
      GPIO_SetBits(RELAY_SEND_PORT, RELAY_SEND_PIN);
      GPIO_SetBits(ADM_OUT_DE_PORT, ADM_OUT_DE_PIN);
      wait_ms(10);
        
      //update LCD
      char szene_id_ascii = 49U + dmx.scene_id;
      strcpy(lcd.line1, str_recall_1);
      strcpy(lcd.line2, str_recall_2);
      strcat(lcd.line2, &szene_id_ascii); 
      lcd.repaint = LCD_REPAINT;
    }
    
    //set LEDs on user button
    iox_set_led(0x01 << dmx.scene_id);
    
    break;
    
  case ST_DMX_MASTER:    
    //if a state change was requested then process it. otherwise just do nothing in this state
    //dmx usart is handled by DMA
    if(recall_buttons != 0x00 || 
       menu_buttons & MENU_BUTTON_RECORD  ||
       menu_buttons & MENU_BUTTON_UP ||
       menu_buttons & MENU_BUTTON_DOWN ||
       dmx.transmitter.status == DMX_TRANSMIT_STOP_REQUESTED){
      
      //Recall was requested
      //Stop transmit, and wait until stopped before loading scene
      dmx_request_stop_transmit();
      if(dmx.transmitter.status == DMX_TRANSMIT_STOPPED){
        //Handle Front Buttons
        if(recall_buttons != 0x00){
          state = ST_PROCESS_FRONT_BUTTONS;
        //Handle Rack Buttons
        }else if(menu_buttons & MENU_BUTTON_UP){
          if(dmx.scene_id < 7){
            dmx.scene_id++;
            state = ST_LOAD_SCENE;
          }
          menu_buttons = 0x00;
        }else if(menu_buttons & MENU_BUTTON_DOWN){
          if(dmx.scene_id > 0){
            dmx.scene_id--;
            state = ST_LOAD_SCENE;
          }
          menu_buttons = 0x00;
        }else if(menu_buttons & MENU_BUTTON_RECORD){
          //disconnect transmit driver from bus
          GPIO_ResetBits(RELAY_SEND_PORT, RELAY_SEND_PIN);
          GPIO_ResetBits(ADM_OUT_DE_PORT, ADM_OUT_DE_PIN);
          
          state = ST_RECORD_PRE;
          menu_buttons = 0x00;
          dmx.recorder.scene_id = DMX_SCENE_ID_INVALID;
        }
      }
    }
    break;
  
  case ST_PASSTHROUGH:
    //Passthrough button pressed --> Reset Buttons, but stay
    if(recall_buttons == 0x80){
      recall_buttons = 0x00;
    //other button pressed --> handle them
    }else if(recall_buttons != 0x00){
      state=ST_PROCESS_FRONT_BUTTONS;
    }else if(menu_buttons & MENU_BUTTON_DOWN){
      if(dmx.scene_id > 0){
        dmx.scene_id--;
        state = ST_LOAD_SCENE;
      }
      menu_buttons = 0x00;
    }else if(menu_buttons & MENU_BUTTON_RECORD){
      state = ST_RECORD_PRE;
      menu_buttons = 0x00;
      dmx.recorder.scene_id = DMX_SCENE_ID_INVALID;
    }
    break;
    
  case ST_RECORD_PRE:    
    //This is the page to select the record slot
    if(menu_buttons & MENU_BUTTON_UP || menu_buttons & MENU_BUTTON_DOWN || 
       dmx.recorder.scene_id == DMX_SCENE_ID_INVALID){
      
      //default scene is 0
      if(dmx.recorder.scene_id == DMX_SCENE_ID_INVALID){      
         dmx.recorder.scene_id = 0;   
      }
      
      //increase decrease save slot position on buttonpress
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
      
      //update LCD with current slot
      char szene_id_ascii_save = 49U + dmx.recorder.scene_id;  // 49U = ASCII '0'
      strcpy(lcd.line1, str_record_1);
      strcpy(lcd.line2, str_record_2);
      strcat(lcd.line1, &szene_id_ascii_save); 
      lcd.repaint = LCD_REPAINT;
      
    }else if(menu_buttons & MENU_BUTTON_RECORD){
      //Set record relay and wait for frame
      GPIO_SetBits(RELAY_RECEIVE_PORT, RELAY_RECEIVE_PIN);
      GPIO_ResetBits(ADM_IN_NRE_PORT, ADM_IN_NRE_PIN);
      wait_ms(20);
      state = ST_RECORD;
      //dmx recorder has its own substates
      dmx.recorder.status = DMX_RECORD_SearchFrame1;
      menu_buttons = 0;
    }else if(menu_buttons & MENU_BUTTON_EXIT){
      //go back and keep current scene
      menu_buttons = 0;
      state=ST_LOAD_SCENE;
    }
    break;
    
  case ST_RECORD:
    //switch substates
    switch(dmx.recorder.status){
    case DMX_RECORD_SearchFrame1:
      dmx.recorder.frame_found = DMX_RECORDER_NO_FRAME_FOUND;
      dmx.recorder.end_time = millis + DMX_TIMEOUT_MILLIS; 
      
      strcpy(lcd.line1, str_record_searchframe_1);
      strcpy(lcd.line2, str_null);
      lcd.repaint = LCD_REPAINT;
      
      //start searching for break and sampling data with dma
      dmx_capture_frame();
      
      dmx.recorder.status = DMX_RECORD_SearchFrame2;
      
      break;
      
    case DMX_RECORD_SearchFrame2:
      //no packet found --> timeout
      if(millis > dmx.recorder.end_time){
        strcpy(lcd.line1, str_record_timeout_1);
        strcpy(lcd.line2, str_record_timeout_2);
        lcd.repaint = LCD_REPAINT;
        dmx.recorder.status = DMX_RECORD_TIMEOUT;
      }else{
        //frame found --> tell it to the user
        if(dmx.recorder.frame_found == DMX_RECORDER_FRAME_FOUND){
          strcpy(lcd.line1, str_record_foundframe_1);
          strcpy(lcd.line2, str_record_foundframe_2);
          lcd.repaint = LCD_REPAINT;
         
          dmx.recorder.status = DMX_RECORD_SaveFrame;
        }
      }
      break;
      
    case DMX_RECORD_SaveFrame:
      //wait, so that user can read message on display
      wait_ms(1000);
      
      save_scene(dmx.recorder.scene_id);
      dmx.recorder.status = DMX_RECORD_Done;
      break;
      
    case DMX_RECORD_Done:
      //tell user that all worked well and open relays for reading from the dmx bus
      strcpy(lcd.line1, str_record_savedframe_1);
      strcpy(lcd.line2, str_record_savedframe_2);
      lcd.repaint = LCD_REPAINT;
  
      GPIO_ResetBits(RELAY_RECEIVE_PORT, RELAY_RECEIVE_PIN);
      GPIO_SetBits(ADM_IN_NRE_PORT, ADM_IN_NRE_PIN);
      
      //relay wait time
      wait_ms(20);
      
      state = ST_RECORD_POST;
      
      break;
      
    case DMX_RECORD_TIMEOUT:
      //user has to confirm message with exit button
      if(menu_buttons & MENU_BUTTON_EXIT){
        GPIO_ResetBits(RELAY_RECEIVE_PORT, RELAY_RECEIVE_PIN);
        GPIO_SetBits(ADM_IN_NRE_PORT, ADM_IN_NRE_PIN);
        
        state = ST_LOAD_SCENE;
        menu_buttons = 0;
      }
    }
    break;
    
    case ST_RECORD_POST:
      //user has to confirm message with exit button
      if(menu_buttons & MENU_BUTTON_EXIT){
        dmx.scene_id = dmx.recorder.scene_id;
        state = ST_LOAD_SCENE;
        menu_buttons = 0;
      }
    break;
  
  default:
    break;
  }
}