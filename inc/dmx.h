#ifndef DMX_H
#define DMX_H

#define DMX_PERIOD_US   50000
#define DMX_BREAK_TIME_US 176
#define DMX_MARK_AFTER_BREAK_US 20
#ifdef MODULE_DMX
tDmx dmx;
void load_scene(uint8_t scene_id);

#else
extern tDmx dmx;
extern void load_scene(uint8_t scene_id);

#endif
#endif