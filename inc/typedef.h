#ifndef TYPEDEF_H
#define TYPEDEF_H

#define DMX_PACKET_LEN 513U
#define DMX_RECORDER_FRAME_FOUND 0x01
#define DMX_RECORDER_NO_FRAME_FOUND 0x00

typedef struct{
  uint8_t start;
  uint8_t data[DMX_PACKET_LEN-1];
}tDmx_packet;

typedef enum {
  DMX_TRANSMIT_STOPPED,
  DMX_TRANSMIT_STOP_REQUESTED,
  DMX_TRANSMIT_TRANSMITTING  
}tDmx_transmitter_status;

typedef struct {
  tDmx_transmitter_status status;
}tDmx_transmitter;

typedef enum{
  DMX_RECORD_SearchFrame1,
  DMX_RECORD_SearchFrame2,
  DMX_RECORD_SaveFrame,
  DMX_RECORD_Done,
  DMX_RECORD_TIMEOUT
}tDmx_recorder_status;

typedef struct tDmx_recorder{
  uint8_t scene_id;     //save position
  uint64_t end_time;    //for timeout
  
  uint8_t frame_found;
  
  tDmx_recorder_status status;
}tDmx_recorder;

typedef struct{
  tDmx_packet buf;
  tDmx_transmitter transmitter;
  tDmx_recorder recorder;
  uint8_t scene_id;
}tDmx;

typedef enum{
  LCD_NO_REPAINT,
  LCD_REPAINT
}tLCDStates;

typedef struct{
  char line1[17];    //16 + Nullterminator
  char line2[17];
  tLCDStates repaint;
}tLCD;

typedef enum{
  ST_BOOT,
  ST_PROCESS_FRONT_BUTTONS,
  ST_LOAD_SCENE,
  ST_DMX_MASTER,
  ST_PASSTHROUGH,
  ST_RECORD_PRE,
  ST_RECORD,
  ST_RECORD_POST
}tStates;

#endif
