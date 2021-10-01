#ifndef DMX_H
#define DMX_H

#define DMX_PERIOD_US   50000
#define DMX_BREAK_TIME_US 176
#define DMX_MARK_AFTER_BREAK_US 20

#define DMX_TIMEOUT_MILLIS 5000;

#define DMX_SCENE_ID_INVALID 0xFF
#ifdef MODULE_DMX
tDmx dmx;
void load_scene(uint8_t scene_id);
void dmx_start_transmit();
void dmx_request_stop_transmit();
#else
extern tDmx dmx;
extern void load_scene(uint8_t scene_id);
extern void dmx_start_transmit();
extern void dmx_request_stop_transmit();
#endif
#endif