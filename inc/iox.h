#ifndef IOX_H
#define IOX_H

#ifdef MODULE_IOX
void i2c_transmit_data(I2C_TypeDef *I2Cx, uint8_t i2c_addr, uint8_t *buf, uint8_t size);
void init_i2c_iox();
void iox_set_led(uint8_t led);
#else
extern void init_i2c_iox();
extern void iox_set_led(uint8_t led);
#endif
#endif