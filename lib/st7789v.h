/**
 * @file st7789v.h
 * @brief ST7789v Display Controller Library for 76×284 TFT LCD
 * @author Ale Moglia
 * @date 2025
 *
 * Optimized library for ST7789v variant display controller with:
 * - Model: ST7789v (MIPI SPI variant)
 * - Physical dimensions: 76×264 pixels
 * - Rotated dimensions: 284×76 pixels (90° rotation)
 * - Coordinate offsets: width=18, height=82
 * - Working coordinate ranges discovered through empirical testing
 *
 * Hardware Configuration:
 * - Platform: MIPI SPI interface
 * - Buffer: 100% framebuffer support
 * - Color depth: 16-bit RGB565
 * - Color inversion: INVOFF required (not INVON)
 *
 * Key Features:
 * - Raw pixel drawing with optimized SPI communication
 * - Coordinate translation for proper display mapping
 * - Screen clearing across entire virtual coordinate space
 * - Custom bitmap font rendering system
 * - Hardware-specific color inversion handling
 */

#ifndef ST7789V_H
#define ST7789V_H

#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

#ifdef __cplusplus
extern "C"
{
#endif

// ==================== ST7789v Display Configuration ====================

/** @brief ST7789v Physical Display Specifications */
#define ST7789V_PHYSICAL_WIDTH 76   ///< Physical width in pixels (narrow dimension)
#define ST7789V_PHYSICAL_HEIGHT 264 ///< Physical height in pixels (tall dimension)

/** @brief ST7789v Rotated Display Specifications (90° rotation) */
#define ST7789V_ROTATED_WIDTH 284 ///< Width after 90° rotation
#define ST7789V_ROTATED_HEIGHT 76 ///< Height after 90° rotation

/** @brief ST7789v Coordinate Offsets (from empirical web data) */
#define ST7789V_WIDTH_OFFSET 18  ///< Width coordinate offset
#define ST7789V_HEIGHT_OFFSET 82 ///< Height coordinate offset

/** @brief Working Coordinate Ranges (discovered through testing) */
#define ST7789V_WORKING_X_MIN 60  ///< Minimum working X coordinate
#define ST7789V_WORKING_X_MAX 125 ///< Maximum working X coordinate
#define ST7789V_WORKING_Y_MIN 40  ///< Minimum working Y coordinate
#define ST7789V_WORKING_Y_MAX 250 ///< Maximum working Y coordinate

/** @brief Virtual Coordinate Space for Complete Clearing */
#define ST7789V_CLEAR_X_MAX 320 ///< Maximum X for screen clearing
#define ST7789V_CLEAR_Y_MAX 400 ///< Maximum Y for screen clearing

/** @brief ST7789v SPI Commands */
#define ST7789V_CMD_CASET 0x2A  ///< Column Address Set command
#define ST7789V_CMD_RASET 0x2B  ///< Row Address Set command
#define ST7789V_CMD_RAMWR 0x2C  ///< RAM Write command
#define ST7789V_CMD_INVOFF 0x20 ///< Display Inversion OFF command
#define ST7789V_CMD_INVON 0x21  ///< Display Inversion ON command

/** @brief Color Definitions (16-bit RGB565) */
#define ST7789V_COLOR_BLACK 0x0000   ///< Black color
#define ST7789V_COLOR_WHITE 0xFFFF   ///< White color
#define ST7789V_COLOR_RED 0xF800     ///< Red color
#define ST7789V_COLOR_GREEN 0x07E0   ///< Green color
#define ST7789V_COLOR_BLUE 0x001F    ///< Blue color
#define ST7789V_COLOR_YELLOW 0xFFE0  ///< Yellow color (Red + Green)
#define ST7789V_COLOR_CYAN 0x07FF    ///< Cyan color (Green + Blue)
#define ST7789V_COLOR_MAGENTA 0xF81F ///< Magenta color (Red + Blue)

    // ==================== ST7789v Configuration Structure ====================

    /**
     * @brief ST7789v Display Configuration Structure
     *
     * Contains all configuration parameters and GPIO pin assignments
     * for the ST7789v display controller.
     */
    typedef struct
    {
        spi_inst_t *spi_instance;  ///< SPI peripheral instance (spi0 or spi1)
        uint8_t pin_dc;            ///< Data/Command pin (GPIO number)
        uint8_t pin_cs;            ///< Chip Select pin (GPIO number)
        uint8_t pin_rst;           ///< Reset pin (GPIO number)
        uint8_t pin_sck;           ///< SPI Clock pin (GPIO number)
        uint8_t pin_mosi;          ///< SPI MOSI pin (GPIO number)
        uint8_t pin_backlight;     ///< Backlight control pin (GPIO number, optional)
        bool backlight_active_low; ///< True if backlight is active low
        bool color_inversion;      ///< True to enable color inversion (INVON)
    } ST7789V_Config_t;

    // ==================== ST7789v Function Declarations ====================

    /**
     * @brief Initialize ST7789v display with specified configuration
     *
     * Configures GPIO pins, initializes SPI communication, performs display
     * initialization sequence, and sets up coordinate system.
     *
     * @param config Pointer to ST7789v configuration structure
     * @return true if initialization successful, false otherwise
     */
    bool ST7789V_Init(const ST7789V_Config_t *config);

    /**
     * @brief Draw a single pixel at specified coordinates
     *
     * Uses optimized SPI communication to draw a single pixel with
     * automatic coordinate translation for the ST7789v display mapping.
     *
     * @param x X coordinate (will be translated to working range)
     * @param y Y coordinate (will be translated to working range)
     * @param color 16-bit RGB565 color value
     */
    void ST7789V_DrawPixel(uint16_t x, uint16_t y, uint16_t color);

    /**
     * @brief Draw a pixel using raw display coordinates
     *
     * Draws directly to display without coordinate translation.
     * Use for maximum performance when coordinates are pre-calculated.
     *
     * @param x Raw X coordinate in display coordinate space
     * @param y Raw Y coordinate in display coordinate space
     * @param color 16-bit RGB565 color value
     */
    void ST7789V_DrawRawPixel(uint16_t x, uint16_t y, uint16_t color);

    /**
     * @brief Clear entire display to specified color
     *
     * Clears the complete virtual coordinate space to eliminate any
     * residual noise or artifacts from previous display content.
     *
     * @param color 16-bit RGB565 color value (typically ST7789V_COLOR_BLACK)
     */
    void ST7789V_ClearScreen(uint16_t color);

    /**
     * @brief Draw a filled rectangle
     *
     * Draws a solid rectangle using translated coordinates.
     *
     * @param x X coordinate of top-left corner
     * @param y Y coordinate of top-left corner
     * @param width Rectangle width in pixels
     * @param height Rectangle height in pixels
     * @param color 16-bit RGB565 color value
     */
    void ST7789V_DrawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);

    /**
     * @brief Draw a single character using 5×7 bitmap font
     *
     * Renders a character using built-in bitmap font patterns.
     * Supports characters: A-Z, 0-9, and basic symbols.
     *
     * @param c Character to draw
     * @param x X coordinate for character top-left
     * @param y Y coordinate for character top-left
     * @param color 16-bit RGB565 color value
     * @return Character width in pixels (6 including spacing)
     */
    uint8_t ST7789V_DrawChar(char c, uint16_t x, uint16_t y, uint16_t color);

    /**
     * @brief Draw a text string using bitmap font
     *
     * Renders a null-terminated string with automatic character spacing.
     *
     * @param text Null-terminated string to draw
     * @param x X coordinate for text start
     * @param y Y coordinate for text start
     * @param color 16-bit RGB565 color value
     * @return Total text width in pixels
     */
    uint16_t ST7789V_DrawText(const char *text, uint16_t x, uint16_t y, uint16_t color);

    /**
     * @brief Draw a text string using medium bitmap font (10×14)
     *
     * Renders a null-terminated string with automatic character spacing
     * using the larger, more visible medium font.
     *
     * @param text Null-terminated string to draw
     * @param x X coordinate for text start
     * @param y Y coordinate for text start
     * @param color 16-bit RGB565 color value
     * @return Total text width in pixels
     */
    uint16_t ST7789V_DrawTextMedium(const char *text, uint16_t x, uint16_t y, uint16_t color);

    /**
     * @brief Draw a single character using medium bitmap font (10×14)
     *
     * Renders a character using built-in medium bitmap font patterns.
     * Supports characters: A-Z and space.
     *
     * @param c Character to draw
     * @param x X coordinate for character top-left
     * @param y Y coordinate for character top-left
     * @param color 16-bit RGB565 color value
     * @return Character width in pixels (12 including spacing)
     */
    uint8_t ST7789V_DrawCharMedium(char c, uint16_t x, uint16_t y, uint16_t color);

    /**
     * @brief Fast clear entire display to specified color
     *
     * Optimized screen clearing using bulk SPI operations instead of
     * pixel-by-pixel clearing for much faster performance.
     *
     * @param color 16-bit RGB565 color value (typically ST7789V_COLOR_BLACK)
     */
    void ST7789V_FastClearScreen(uint16_t color);

    /**    /**
     * @brief Set backlight state
     *
     * Controls display backlight if backlight pin is configured.
     *
     * @param enabled True to turn backlight on, false to turn off
     */
    void ST7789V_SetBacklight(bool enabled);

    /**
     * @brief Set display color inversion
     *
     * Enables or disables hardware color inversion.
     * Note: ST7789v typically requires INVOFF for correct colors.
     *
     * @param inverted True to enable inversion (INVON), false to disable (INVOFF)
     */
    void ST7789V_SetColorInversion(bool inverted);

    /**
     * @brief Translate logical coordinates to display coordinates
     *
     * Converts user coordinates to the working coordinate range
     * discovered for this ST7789v variant.
     *
     * @param logical_x Logical X coordinate (0 to usable width)
     * @param logical_y Logical Y coordinate (0 to usable height)
     * @param display_x Pointer to store translated X coordinate
     * @param display_y Pointer to store translated Y coordinate
     */
    void ST7789V_TranslateCoordinates(uint16_t logical_x, uint16_t logical_y,
                                      uint16_t *display_x, uint16_t *display_y);

    /**
     * @brief Get usable display dimensions
     *
     * Returns the usable display area based on discovered working coordinate ranges.
     *
     * @param width Pointer to store usable width
     * @param height Pointer to store usable height
     */
    void ST7789V_GetUsableDimensions(uint16_t *width, uint16_t *height);

#ifdef __cplusplus
}
#endif

#endif // ST7789V_H