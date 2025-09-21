/**
 * @file st7789.h
 * @brief ST7789 TFT LCD Driver for Raspberry Pi Pico
 * @author Ale Moglia (modified from original library)
 * @date 2025
 *
 * This library provides hardware-specific functions for controlling ST7789 TFT LCD displays
 * via SPI interface on the Raspberry Pi Pico microcontroller.
 */

#ifndef ST7789_H
#define ST7789_H

#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

// Function declarations
void LCD_setPins(uint16_t dc, uint16_t cs, int16_t rst, uint16_t sck, uint16_t tx);
void LCD_setSPIperiph(spi_inst_t *s);
void LCD_initDisplay(uint16_t width, uint16_t height);
void LCD_setRotation(uint8_t m);
void LCD_setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void LCD_WritePixel(int x, int y, uint16_t col);
void LCD_WriteBitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *bitmap);
void ST7789_SendCommand(uint8_t commandByte, const uint8_t *dataBytes, uint8_t numDataBytes);

/**
 * @brief Initialize 76x264 ST7789 display with optimized settings
 * @param width Display width (should be 76)
 * @param height Display height (should be 264)
 * @return true if successful, false if dimensions don't match
 */
bool LCD_init76x264Display(uint16_t width, uint16_t height);
#endif