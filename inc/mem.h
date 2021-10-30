#ifndef MEM_H
#define MEM_H

#ifdef MODULE_MEM
void init_mem();
void write_to_eeprom(uint16_t address, uint8_t* buf, uint16_t len);
void read_from_eeprom(uint16_t address, uint8_t* buf, uint16_t len);

#else
void init_mem();
extern void write_to_eeprom(uint16_t address, uint8_t* buf, uint16_t len);
extern void read_from_eeprom(uint16_t address, uint8_t* buf, uint16_t len);
#endif
#endif