
#ifndef OLED_SSD1306_H
#define OLED_SSD1306_H

#include <stdint.h>

/**
 * @brief ssd1306 OLED Initialize.
 */
uint32_t OledInit (void);

/**
 * @brief Set cursor position
 *
 * @param x the horizontal posistion of cursor
 * @param y the vertical position of cursor 
 * @return Returns {@link WIFI_IOT_SUCCESS} if the operation is successful;
 * returns an error code defined in {@link wifiiot_errno.h} otherwise.
 */
void OledSetPosition (uint8_t x, uint8_t y);

void OledFillScreen (uint8_t fillData);

enum Font
{
	FONT6x8 = 1,
	FONT8x16
};
typedef enum Font Font;

void OledShowChar (uint8_t x, uint8_t y, uint8_t ch, Font font);
void OledShowString (uint8_t x, uint8_t y, const char *str, Font font);

#endif // OLED_SSD1306_H
