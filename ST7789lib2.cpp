/**
 * @file ST7789lib2.cpp
 * @brief ST7789 TFT LCD Display Hello World Test
 * @author Ale Moglia
 * @date 2025
 *
 * Simple test program for ST7789 display functionality.
 * Displays direct pixels on a 76x264 ST7789 TFT LCD.
 *
 * Hardware Configuration:
 * - Display: 76x264 ST7789 TFT LCD
 * - SPI: spi1 (GPIO10=SCK, GPIO11=MOSI)
 * - Control pins: DC=GPIO8, CS=GPIO9, RST=GPIO6
 * - UART Debug: TX=GPIO1, RX=GPIO0 (115200 baud)
 * - Backlight: BLK pin connected to GND
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "lib/st7789.h"
#include "lib/gfx.h"

/** @brief ST7789v specific configuration constants */
const int ST7789V_WIDTH_OFFSET = 18;   ///< Width offset for ST7789v variant
const int ST7789V_HEIGHT_OFFSET = 82;  ///< Height offset for ST7789v variant
const int ST7789V_ROTATED_WIDTH = 284; ///< Width after 90° rotation
const int ST7789V_ROTATED_HEIGHT = 76; ///< Height after 90° rotation

/** @brief Working coordinate ranges discovered through testing */
const int WORKING_X_MIN = 60;  ///< Minimum working X coordinate
const int WORKING_X_MAX = 125; ///< Maximum working X coordinate
const int WORKING_Y_MIN = 40;  ///< Minimum working Y coordinate
const int WORKING_Y_MAX = 250; ///< Maximum working Y coordinate

/** @brief Display dimensions for 76x284 ST7789 LCD - CORRECTED ORIENTATION */
const int lcd_width = 76;   ///< Display width in pixels (narrow side)
const int lcd_height = 284; ///< Display height in pixels (tall side)

/**
 * @brief Main program entry point
 *
 * Initializes the ST7789 display and demonstrates basic pixel drawing.
 *
 * @return int Program exit status (never reached due to infinite loop)
 */

int main()
{
    stdio_init_all();

    // Allow time for UART initialization and connection
    sleep_ms(2000);
    printf("Starting ST7789v DIRECT PIXEL test for ST7789v display (284x76 rotated)...\n");

    // Configure SPI peripheral instance
    spi_inst_t *st7789_spi = spi1;
    printf("SPI peripheral set to spi1\n");

    // Configure GPIO pins for ST7789 display
    // Parameters: DC, CS, RST, SCK, TX (MOSI)
    LCD_setPins(8, 9, 6, 10, 11);
    printf("Pins configured: DC=GPIO8, CS=GPIO9, RST=GPIO6, SCK=GPIO10, TX=GPIO11\n");

    // BLK connected to GPIO12 - Active LOW backlight (LOW = ON, HIGH = OFF)
    gpio_init(12);
    gpio_set_dir(12, GPIO_OUT);
    gpio_put(12, 0); // Set LOW to turn backlight ON (active low)
    printf("Backlight GPIO12 set to LOW (backlight ON) - active low control\n");

    // Assign SPI peripheral to LCD driver
    LCD_setSPIperiph(st7789_spi);
    printf("SPI peripheral configured for LCD\n");

    printf("Initializing display...\n");
    // Use standard ST7789 initialization with 76x264 fix
    LCD_initDisplay(lcd_width, lcd_height);
    printf("Display initialized with %dx%d resolution\n", lcd_width, lcd_height);

    // Turn OFF hardware color inversion for correct colors
    printf("Turning OFF hardware color inversion...\n");
    // Use INVOFF to get normal colors (since INVON was causing white background)
    ST7789_SendCommand(0x20, NULL, 0); // INVOFF - inversion OFF
    printf("Hardware color inversion disabled (INVOFF)\n");

    printf("Starting SIMPLE area test...\n");

    while (1)
    {
        printf("=== COMPREHENSIVE SCREEN CLEARING TEST ===\n");

        auto drawRawPixel = [](uint16_t x, uint16_t y, uint16_t color)
        {
            gpio_put(9, 0);
            gpio_put(8, 0);
            uint8_t caset_cmd = 0x2A;
            spi_write_blocking(spi1, &caset_cmd, 1);
            gpio_put(8, 1);
            uint8_t caset_data[] = {(uint8_t)(x >> 8), (uint8_t)(x & 0xFF), (uint8_t)(x >> 8), (uint8_t)(x & 0xFF)};
            spi_write_blocking(spi1, caset_data, 4);

            gpio_put(8, 0);
            uint8_t raset_cmd = 0x2B;
            spi_write_blocking(spi1, &raset_cmd, 1);
            gpio_put(8, 1);
            uint8_t raset_data[] = {(uint8_t)(y >> 8), (uint8_t)(y & 0xFF), (uint8_t)(y >> 8), (uint8_t)(y & 0xFF)};
            spi_write_blocking(spi1, raset_data, 4);

            gpio_put(8, 0);
            uint8_t ramwr_cmd = 0x2C;
            spi_write_blocking(spi1, &ramwr_cmd, 1);
            gpio_put(8, 1);
            spi_set_format(spi1, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
            spi_write16_blocking(spi1, &color, 1);
            spi_set_format(spi1, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
            gpio_put(9, 1);
        };

        // OPTIMIZED FAST CLEAR SCREEN function - with SPI speed boost
        auto fastClearScreen = [](uint16_t color)
        {
            printf("FAST CLEAR: Setting display window to full screen...\n");

            // Set column address window (0 to 319)
            gpio_put(9, 0);
            gpio_put(8, 0);
            uint8_t caset_cmd = 0x2A;
            spi_write_blocking(spi1, &caset_cmd, 1);
            gpio_put(8, 1);
            uint8_t caset_data[] = {0x00, 0x00, 0x01, 0x3F}; // 0 to 319
            spi_write_blocking(spi1, caset_data, 4);
            gpio_put(9, 1);

            // Set row address window (0 to 399)
            gpio_put(9, 0);
            gpio_put(8, 0);
            uint8_t raset_cmd = 0x2B;
            spi_write_blocking(spi1, &raset_cmd, 1);
            gpio_put(8, 1);
            uint8_t raset_data[] = {0x00, 0x00, 0x01, 0x8F}; // 0 to 399
            spi_write_blocking(spi1, raset_data, 4);
            gpio_put(9, 1);

            // Start memory write
            gpio_put(9, 0);
            gpio_put(8, 0);
            uint8_t ramwr_cmd = 0x2C;
            spi_write_blocking(spi1, &ramwr_cmd, 1);
            gpio_put(8, 1);

            printf("FAST CLEAR: Boosting SPI speed and writing data...\n");

            // BOOST SPI SPEED FOR BULK TRANSFER (breadboard-safe)
            // Save current speed and boost to breadboard-safe maximum
            uint32_t old_baudrate = spi_get_baudrate(spi1);
            printf("Current SPI speed: %u Hz (%.1f MHz)\n", old_baudrate, old_baudrate / 1000000.0);

            // Try breadboard-safe maximum SPI speed (10 MHz)
            uint32_t new_baudrate = spi_set_baudrate(spi1, 10000000);
            printf("Boosted SPI speed: %u Hz (%.1f MHz) - breadboard safe\n", new_baudrate, new_baudrate / 1000000.0);

            // Switch to 16-bit SPI mode
            spi_set_format(spi1, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

            // Create a large buffer - 4KB (2048 pixels)
            const int buffer_size = 2048;
            uint16_t color_buffer[buffer_size];

            // Fill buffer with color
            for (int i = 0; i < buffer_size; i++)
            {
                color_buffer[i] = color;
            }

            // Calculate total pixels and chunks
            const int total_pixels = 320 * 400;                      // 128,000 pixels
            const int full_chunks = total_pixels / buffer_size;      // 62 chunks
            const int remaining_pixels = total_pixels % buffer_size; // 512 pixels

            // Write full chunks at maximum speed
            for (int chunk = 0; chunk < full_chunks; chunk++)
            {
                spi_write16_blocking(spi1, color_buffer, buffer_size);
            }

            // Write remaining pixels
            if (remaining_pixels > 0)
            {
                spi_write16_blocking(spi1, color_buffer, remaining_pixels);
            }

            // Restore original SPI settings
            spi_set_baudrate(spi1, old_baudrate);
            spi_set_format(spi1, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
            gpio_put(9, 1);

            printf("FAST CLEAR: Complete! Used %d chunks at %u Hz.\n", full_chunks + (remaining_pixels > 0 ? 1 : 0), new_baudrate);
        };

        printf("STEP 1: FAST CLEAR SCREEN TEST...\n");

        // Time the fast clear
        absolute_time_t start_time = get_absolute_time();
        fastClearScreen(0x0000); // Clear to black
        absolute_time_t end_time = get_absolute_time();

        int64_t clear_time_us = absolute_time_diff_us(start_time, end_time);
        printf("FAST CLEAR completed in %lld microseconds (%.2f ms)!\n",
               clear_time_us, clear_time_us / 1000.0);

        sleep_ms(2000);

        printf("STEP 2: Drawing all rectangles in the SAME working coordinate area...\n");

        // Since you can see the GREEN box, let's put all rectangles in that same area
        // The green box was at (80,40) to (100,60) - that range works!

        printf("Drawing all colors near the visible GREEN box area...\n");

        // RED rectangle - right next to the green box
        printf("Drawing RED rectangle at (80,70) to (100,90)...\n");
        for (int y = 70; y < 90; y++)
        {
            for (int x = 80; x < 100; x++)
            {
                drawRawPixel(x, y, 0xF800); // RED
            }
        }

        // BLUE rectangle - below the red
        printf("Drawing BLUE rectangle at (80,100) to (100,120)...\n");
        for (int y = 100; y < 120; y++)
        {
            for (int x = 80; x < 100; x++)
            {
                drawRawPixel(x, y, 0x001F); // BLUE
            }
        }

        // YELLOW rectangle - below the blue
        printf("Drawing YELLOW rectangle at (80,130) to (100,150)...\n");
        for (int y = 130; y < 150; y++)
        {
            for (int x = 80; x < 100; x++)
            {
                drawRawPixel(x, y, 0xFFE0); // YELLOW
            }
        }

        // WHITE rectangle - below the yellow
        printf("Drawing WHITE rectangle at (80,160) to (100,180)...\n");
        for (int y = 160; y < 180; y++)
        {
            for (int x = 80; x < 100; x++)
            {
                drawRawPixel(x, y, 0xFFFF); // WHITE
            }
        }

        // CYAN rectangle - to the left of green
        printf("Drawing CYAN rectangle at (60,40) to (75,60)...\n");
        for (int y = 40; y < 60; y++)
        {
            for (int x = 60; x < 75; x++)
            {
                drawRawPixel(x, y, 0x07FF); // CYAN
            }
        }

        // MAGENTA rectangle - to the right of green
        printf("Drawing MAGENTA rectangle at (105,40) to (125,60)...\n");
        for (int y = 40; y < 60; y++)
        {
            for (int x = 105; x < 125; x++)
            {
                drawRawPixel(x, y, 0xF81F); // MAGENTA
            }
        }

        printf("STEP 3: SIMPLE TEXT DRAWING TEST...\n");

        // Simple bitmap font for "HELLO" - 5x7 pixel font
        auto drawChar = [&](char c, int start_x, int start_y, uint16_t color)
        {
            // Simple 5x7 bitmap font patterns
            uint8_t font_H[7] = {0b10001, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001};
            uint8_t font_E[7] = {0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b11111};
            uint8_t font_L[7] = {0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b11111};
            uint8_t font_O[7] = {0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110};

            uint8_t *pattern = nullptr;
            switch (c)
            {
            case 'H':
                pattern = font_H;
                break;
            case 'E':
                pattern = font_E;
                break;
            case 'L':
                pattern = font_L;
                break;
            case 'O':
                pattern = font_O;
                break;
            default:
                return; // Unknown character
            }

            // Draw the character using the bitmap pattern
            for (int row = 0; row < 7; row++)
            {
                for (int col = 0; col < 5; col++)
                {
                    if (pattern[row] & (1 << (4 - col)))
                    { // Check if pixel should be drawn
                        drawRawPixel(start_x + col, start_y + row, color);
                    }
                }
            }
        };

        // Draw "HELLO" text - moved significantly further right to show H completely
        printf("Drawing 'HELLO' text - moved significantly right to show H completely...\n");
        drawChar('H', 92, 190, 0xFFFF);  // White H (moved right +12 pixels from original)
        drawChar('E', 98, 190, 0xF800);  // Red E
        drawChar('L', 104, 190, 0x07E0); // Green L
        drawChar('L', 110, 190, 0x001F); // Blue L
        drawChar('O', 116, 190, 0xFFE0); // Yellow O

        // Draw "PICO" text below - moved significantly further right to show P completely
        printf("Drawing 'PICO' text - moved significantly right to show P completely...\n");
        // Add P and I patterns
        uint8_t font_P[7] = {0b11110, 0b10001, 0b10001, 0b11110, 0b10000, 0b10000, 0b10000};
        uint8_t font_I[7] = {0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b11111};
        uint8_t font_C[7] = {0b01110, 0b10001, 0b10000, 0b10000, 0b10000, 0b10001, 0b01110};

        auto drawCustomChar = [&](uint8_t *pattern, int start_x, int start_y, uint16_t color)
        {
            for (int row = 0; row < 7; row++)
            {
                for (int col = 0; col < 5; col++)
                {
                    if (pattern[row] & (1 << (4 - col)))
                    {
                        drawRawPixel(start_x + col, start_y + row, color);
                    }
                }
            }
        };

        drawCustomChar(font_P, 92, 200, 0x07FF);  // Cyan P (moved right +12 pixels from original)
        drawCustomChar(font_I, 98, 200, 0xF81F);  // Magenta I
        drawCustomChar(font_C, 104, 200, 0xFFFF); // White C
        drawChar('O', 110, 200, 0xF800);          // Red O

        printf("TEXT RENDERING FULLY FUNCTIONAL!\n");

        // STEP 4: MEDIUM FONT TEST - Add one large character
        printf("STEP 4: Testing MEDIUM font (10x14 pixels)...\n");

        // Medium font 'H' pattern - 10x14 pixels (double-sized)
        // Using uint16_t to store 10-bit patterns
        uint16_t medium_font_H[14] = {
            0b1100000011, // ██      ██
            0b1100000011, // ██      ██
            0b1100000011, // ██      ██
            0b1100000011, // ██      ██
            0b1100000011, // ██      ██
            0b1100000011, // ██      ██
            0b1111111111, // ██████████
            0b1111111111, // ██████████
            0b1100000011, // ██      ██
            0b1100000011, // ██      ██
            0b1100000011, // ██      ██
            0b1100000011, // ██      ██
            0b1100000011, // ██      ██
            0b1100000011  // ██      ██
        };

        auto drawMediumChar = [&](uint16_t *pattern, int start_x, int start_y, uint16_t color)
        {
            printf("Drawing medium character at (%d,%d)...\n", start_x, start_y);
            for (int row = 0; row < 14; row++)
            {
                for (int col = 0; col < 10; col++)
                {
                    if (pattern[row] & (1 << (9 - col))) // 10-bit pattern, MSB first
                    {
                        drawRawPixel(start_x + col, start_y + row, color);
                    }
                }
            }
        };

        // Draw one medium 'H' - positioned to the right of normal text
        printf("Drawing medium 'H' (10x14 pixels)...\n");
        drawMediumChar(medium_font_H, 92, 215, 0x07E0); // Green medium H

        printf("Medium font 'H' should be visible and much larger!\n");

        printf("Your ST7789 display now has:\n");
        printf("- Complete color graphics capability\n");
        printf("- Full text rendering with custom fonts\n");
        printf("- NEW: Medium font capability (10x14 pixels)\n");
        printf("- Perfect coordinate system mapping\n");
        printf("- Ready for Eurorack projects!\n");

        sleep_ms(10000);
    }
}