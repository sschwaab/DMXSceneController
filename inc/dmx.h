#ifndef DMX_H
#define DMX_H
#define TIM4_CLEAR_INTERRUPTS TIM4->SR=~(TIM_IT_Update|TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4|TIM_IT_COM|TIM_IT_Trigger|TIM_IT_Break)

//DMX Timing specifications
#define DMX_PERIOD_US   50000
#define DMX_BREAK_TIME_US 176
#define DMX_MARK_AFTER_BREAK_US 20

//Recording Timeout
#define DMX_TIMEOUT_MILLIS 5000;

#define DMX_SCENE_ID_INVALID 0xFF

#ifdef MODULE_DMX
tDmx dmx;
void load_scene(uint8_t scene_id);
void dmx_start_transmit();
void dmx_request_stop_transmit();
void dmx_capture_frame();
void init_dmx(void);
void save_scene(uint8_t scene_id);
#else
extern tDmx dmx;
extern void load_scene(uint8_t scene_id);
extern void dmx_start_transmit();
extern void dmx_request_stop_transmit();
extern void dmx_capture_frame();
extern void init_dmx();
extern void save_scene(uint8_t scene_id);
#endif
#endif