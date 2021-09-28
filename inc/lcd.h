#ifndef LCD_H
#define LCD_H

#define LCD_PORT GPIOA
#define LCD_DB4 GPIO_Pin_0
#define LCD_DB5 GPIO_Pin_1
#define LCD_DB6 GPIO_Pin_2
#define LCD_DB7 GPIO_Pin_3
#define LCD_EN  GPIO_Pin_4
#define LCD_RS  GPIO_Pin_5

#define LCD_WAIT     for(uint16_t i=0; i<300; i++); //ca 45µs @ 48MHz

#ifdef MODULE_LCD

void write4bit(uint8_t nibble);

void init_lcd();
void lcd_setcursor(uint8_t row, uint8_t col);
void lcd_putchars(uint8_t* chr, uint8_t len);
void lcd_write(uint8_t byte);
void lcd_write4bit(uint8_t nibble);
void lcd_clear();

#else

extern void init_lcd();
extern void lcd_setcursor(uint8_t col, uint8_t row);
extern void lcd_putchars(uint8_t* chr, uint8_t len);
extern void lcd_clear();


#endif
#endif