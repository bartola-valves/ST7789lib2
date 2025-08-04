/**
 * @file gfx.h
 * @brief Graphics Library for ST7789 TFT LCD with Framebuffer Support
 * @author Ale Moglia (modified from Adafruit GFX)
 * @date 2025
 *
 * This library provides high-level graphics functions for drawing shapes, text,
 * and managing a framebuffer for ST7789 displays.
 */

#ifndef GFX_H
#define GFX_H

#include "pico/stdlib.h"
#include "gfxfont.h"

/**
 * @brief Convert 8-bit RGB values to 16-bit RGB565 format
 * @param R Red component (0-255)
 * @param G Green component (0-255)
 * @param B Blue component (0-255)
 * @return 16-bit RGB565 color value
 */
#define GFX_RGB565(R, G, B) ((uint16_t)(((R) & 0b11111000) << 8) | (((G) & 0b11111100) << 3) | ((B) >> 3))

// Framebuffer Management
/**
 * @brief Create and allocate memory for the framebuffer
 * @note Must be called before any drawing operations
 */
void GFX_createFramebuf();

/**
 * @brief Destroy and free framebuffer memory
 */
void GFX_destroyFramebuf();

// Basic Drawing Functions
/**
 * @brief Draw a single pixel in the framebuffer
 * @param x X coordinate
 * @param y Y coordinate
 * @param color 16-bit RGB565 color
 */
void GFX_drawPixel(int16_t x, int16_t y, uint16_t color);

// Text Functions
/**
 * @brief Draw a single character
 * @param x X position
 * @param y Y position
 * @param c Character to draw
 * @param color Text color
 * @param bg Background color
 * @param size_x X-axis text scaling
 * @param size_y Y-axis text scaling
 */
void GFX_drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size_x, uint8_t size_y);

/**
 * @brief Write a character at current cursor position
 * @param c Character to write
 */
void GFX_write(uint8_t c);

/**
 * @brief Set text cursor position
 * @param x X coordinate
 * @param y Y coordinate
 */
void GFX_setCursor(int16_t x, int16_t y);

/**
 * @brief Set text color
 * @param color 16-bit RGB565 color
 */
void GFX_setTextColor(uint16_t color);

/**
 * @brief Set text background color
 * @param color 16-bit RGB565 background color
 */
void GFX_setTextBack(uint16_t color);

/**
 * @brief Set text size scaling factor
 * @param size Scaling factor (1=normal, 2=double size, etc.)
 */
void GFX_setTextSize(uint8_t size);

/**
 * @brief Set custom font
 * @param f Pointer to GFXfont structure
 */
void GFX_setFont(const GFXfont *f);

// Line Drawing Functions
/**
 * @brief Draw a line between two points
 * @param x0 Starting X coordinate
 * @param y0 Starting Y coordinate
 * @param x1 Ending X coordinate
 * @param y1 Ending Y coordinate
 * @param color 16-bit RGB565 color
 */
void GFX_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);

/**
 * @brief Draw a fast vertical line
 * @param x X coordinate
 * @param y Starting Y coordinate
 * @param h Height of line
 * @param color 16-bit RGB565 color
 */
void GFX_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);

/**
 * @brief Draw a fast horizontal line
 * @param x Starting X coordinate
 * @param y Y coordinate
 * @param l Length of line
 * @param color 16-bit RGB565 color
 */
void GFX_drawFastHLine(int16_t x, int16_t y, int16_t l, uint16_t color);

// Rectangle Functions
/**
 * @brief Draw a rectangle outline
 * @param x X coordinate of top-left corner
 * @param y Y coordinate of top-left corner
 * @param w Width
 * @param h Height
 * @param color 16-bit RGB565 color
 */
void GFX_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

/**
 * @brief Draw a filled rectangle
 * @param x X coordinate of top-left corner
 * @param y Y coordinate of top-left corner
 * @param w Width
 * @param h Height
 * @param color 16-bit RGB565 color
 */
void GFX_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

// Screen Functions
/**
 * @brief Fill entire screen with a color
 * @param color 16-bit RGB565 color
 */
void GFX_fillScreen(uint16_t color);

/**
 * @brief Set the color used by GFX_clearScreen()
 * @param color 16-bit RGB565 color
 */
void GFX_setClearColor(uint16_t color);

/**
 * @brief Clear screen to the set clear color
 */
void GFX_clearScreen();

// Circle Functions
/**
 * @brief Draw a circle outline
 * @param x0 Center X coordinate
 * @param y0 Center Y coordinate
 * @param r Radius
 * @param color 16-bit RGB565 color
 */
void GFX_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

/**
 * @brief Draw a filled circle
 * @param x0 Center X coordinate
 * @param y0 Center Y coordinate
 * @param r Radius
 * @param color 16-bit RGB565 color
 */
void GFX_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

// Advanced Functions
/**
 * @brief Print formatted text at current cursor position
 * @param format Printf-style format string
 * @param ... Variable arguments
 */
void GFX_printf(const char *format, ...);

/**
 * @brief Flush framebuffer contents to the display
 * @note Call this after drawing operations to update the screen
 */
void GFX_flush();

/**
 * @brief Update display (alias for GFX_flush)
 */
void GFX_Update();

/**
 * @brief Scroll screen content up by n lines
 * @param n Number of lines to scroll
 */
void GFX_scrollUp(int n);

// Utility Functions
/**
 * @brief Get framebuffer width
 * @return Width in pixels
 */
uint GFX_getWidth();

/**
 * @brief Get framebuffer height
 * @return Height in pixels
 */
uint GFX_getHeight();

// Bitmap Functions
/**
 * @brief Draw a monochrome bitmap
 * @param x X coordinate of top-left corner
 * @param y Y coordinate of top-left corner
 * @param bitmap Pointer to bitmap data
 * @param w Width of bitmap in pixels
 * @param h Height of bitmap in pixels
 * @param color Color for set bits (1s)
 * @param bg Background color for unset bits (0s)
 */
void GFX_drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg);

/**
 * @brief Draw a monochrome bitmap with transparent background
 * @param x X coordinate of top-left corner
 * @param y Y coordinate of top-left corner
 * @param bitmap Pointer to bitmap data
 * @param w Width of bitmap in pixels
 * @param h Height of bitmap in pixels
 * @param color Color for set bits (1s), unset bits are transparent
 */
void GFX_drawBitmapMask(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);

// Color Utility Functions
/**
 * @brief Convert RGB888 (8-bit per channel) to RGB565 format
 * @param r Red component (0-255)
 * @param g Green component (0-255)
 * @param b Blue component (0-255)
 * @return 16-bit RGB565 color value
 */
uint16_t GFX_color565(uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Extract red component from RGB565 color
 * @param color RGB565 color value
 * @return Red component (0-255)
 */
uint8_t GFX_getRed(uint16_t color);

/**
 * @brief Extract green component from RGB565 color
 * @param color RGB565 color value
 * @return Green component (0-255)
 */
uint8_t GFX_getGreen(uint16_t color);

/**
 * @brief Extract blue component from RGB565 color
 * @param color RGB565 color value
 * @return Blue component (0-255)
 */
uint8_t GFX_getBlue(uint16_t color);

// Circle Functions
/**
 * @brief Draw a circle outline
 * @param x0 Center X coordinate
 * @param y0 Center Y coordinate
 * @param r Radius in pixels
 * @param color 16-bit RGB565 color
 */
void GFX_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

/**
 * @brief Draw a filled circle
 * @param x0 Center X coordinate
 * @param y0 Center Y coordinate
 * @param r Radius in pixels
 * @param color 16-bit RGB565 color
 */
void GFX_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

#endif