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

// Use DMA for faster transfers (currently disabled)
// #define USE_DMA 1

/** @brief Special signifier for command lists with delays */
#define ST_CMD_DELAY 0x80

// ST77XX Command Definitions
#define ST77XX_NOP 0x00     ///< No operation
#define ST77XX_SWRESET 0x01 ///< Software reset
#define ST77XX_RDDID 0x04   ///< Read display ID
#define ST77XX_RDDST 0x09   ///< Read display status

#define ST77XX_SLPIN 0x10  ///< Enter sleep mode
#define ST77XX_SLPOUT 0x11 ///< Exit sleep mode
#define ST77XX_PTLON 0x12  ///< Partial mode on
#define ST77XX_NORON 0x13  ///< Normal display mode on

#define ST77XX_INVOFF 0x20  ///< Display inversion off
#define ST77XX_INVON 0x21   ///< Display inversion on
#define ST77XX_DISPOFF 0x28 ///< Display off
#define ST77XX_DISPON 0x29  ///< Display on
#define ST77XX_CASET 0x2A   ///< Column address set
#define ST77XX_RASET 0x2B   ///< Row address set
#define ST77XX_RAMWR 0x2C   ///< Memory write
#define ST77XX_RAMRD 0x2E   ///< Memory read

#define ST77XX_PTLAR 0x30  ///< Partial area
#define ST77XX_TEOFF 0x34  ///< Tearing effect line off
#define ST77XX_TEON 0x35   ///< Tearing effect line on
#define ST77XX_MADCTL 0x36 ///< Memory access control
#define ST77XX_COLMOD 0x3A ///< Pixel format set

// Memory Access Control Register bits
#define ST77XX_MADCTL_MY 0x80  ///< Row address order
#define ST77XX_MADCTL_MX 0x40  ///< Column address order
#define ST77XX_MADCTL_MV 0x20  ///< Row/column exchange
#define ST77XX_MADCTL_ML 0x10  ///< Vertical refresh order
#define ST77XX_MADCTL_RGB 0x00 ///< RGB color order

#define ST77XX_RDID1 0xDA ///< Read ID1
#define ST77XX_RDID2 0xDB ///< Read ID2
#define ST77XX_RDID3 0xDC ///< Read ID3
#define ST77XX_RDID4 0xDD ///< Read ID4

/** @brief 16-bit RGB565 color definitions for common colors */
#define ST77XX_BLACK 0x0000   ///< Black color (RGB: 0,0,0)
#define ST77XX_WHITE 0xFFFF   ///< White color (RGB: 255,255,255)
#define ST77XX_RED 0xF800     ///< Red color (RGB: 255,0,0)
#define ST77XX_GREEN 0x07E0   ///< Green color (RGB: 0,255,0)
#define ST77XX_BLUE 0x001F    ///< Blue color (RGB: 0,0,255)
#define ST77XX_CYAN 0x07FF    ///< Cyan color (RGB: 0,255,255)
#define ST77XX_MAGENTA 0xF81F ///< Magenta color (RGB: 255,0,255)
#define ST77XX_YELLOW 0xFFE0  ///< Yellow color (RGB: 255,255,0)
#define ST77XX_ORANGE 0xFC00  ///< Orange color (RGB: 255,128,0)

/**
 * @brief Configure GPIO pins for ST7789 display connection
 * @param dc Data/Command pin (GPIO number)
 * @param cs Chip Select pin (GPIO number)
 * @param rst Reset pin (GPIO number, use -1 if not connected)
 * @param sck SPI Clock pin (GPIO number)
 * @param tx SPI MOSI/TX pin (GPIO number)
 */
void LCD_setPins(uint16_t dc, uint16_t cs, int16_t rst, uint16_t sck, uint16_t tx);

/**
 * @brief Set which SPI peripheral to use
 * @param s Pointer to SPI instance (spi0 or spi1)
 */
void LCD_setSPIperiph(spi_inst_t *s);

/**
 * @brief Initialize the ST7789 display
 * @param width Display width in pixels
 * @param height Display height in pixels
 * @note This function includes specific offset handling for 170x320 displays
 */
void LCD_initDisplay(uint16_t width, uint16_t height);

/**
 * @brief Set display rotation/orientation
 * @param m Rotation mode (0=0°, 1=90°, 2=180°, 3=270°)
 */
void LCD_setRotation(uint8_t m);

/**
 * @brief Write a single pixel to the display
 * @param x X coordinate (0 to width-1)
 * @param y Y coordinate (0 to height-1)
 * @param col 16-bit RGB565 color value
 */
void LCD_WritePixel(int x, int y, uint16_t col);

/**
 * @brief Write a bitmap/image to the display
 * @param x Starting X coordinate
 * @param y Starting Y coordinate
 * @param w Width of the bitmap
 * @param h Height of the bitmap
 * @param bitmap Pointer to 16-bit RGB565 color data array
 */
void LCD_WriteBitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *bitmap);

#endif