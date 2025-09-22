/**
 * @file ST7789v_example.cpp
 * @brief ST7789v Library Usage Example
 * @author Ale Moglia
 * @date 2025
 *
 * Demonstrates usage of the optimized ST7789v library with a 76×284 TFT LCD.
 * This example replaces the previous raw implementation with clean library calls.
 *
 * Hardware Configuration:
 * - Display: 76×264 ST7789v TFT LCD (284×76 when rotated)
 * - SPI: spi1 (GPIO10=SCK, GPIO11=MOSI)
 * - Control pins: DC=GPIO8, CS=GPIO9, RST=GPIO6
 * - Backlight: GPIO12 (active low)
 * - UART Debug: TX=GPIO1, RX=GPIO0 (115200 baud)
 *
 * Features demonstrated:
 * - Library initialization with configuration structure
 * - Screen clearing using optimized coordinate mapping
 * - Colored rectangle drawing with logical coordinates
 * - Text rendering using built-in bitmap fonts
 * - Color inversion management (INVOFF for ST7789v)
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "lib/st7789v.h"

/**
 * @brief Main program entry point
 *
 * Demonstrates all ST7789v library features with a comprehensive test pattern
 * that validates display functionality, coordinate mapping, and text rendering.
 *
 * @return int Program exit status (never reached due to infinite loop)
 */
int main()
{
    stdio_init_all();
    sleep_ms(2000);

    printf("Starting ST7789v Library Example...\n");
    printf("Display: 76×264 ST7789v TFT (284×76 rotated)\n");

    // ==================== ST7789v Configuration ====================

    ST7789V_Config_t display_config = {
        .spi_instance = spi1,         // Use SPI1 peripheral
        .pin_dc = 8,                  // Data/Command pin
        .pin_cs = 9,                  // Chip Select pin
        .pin_rst = 6,                 // Reset pin
        .pin_sck = 10,                // SPI Clock pin
        .pin_mosi = 11,               // SPI MOSI pin
        .pin_backlight = 12,          // Backlight control pin
        .backlight_active_low = true, // Backlight is active low
        .color_inversion = false      // Use INVOFF (recommended for ST7789v)
    };

    // ==================== Display Initialization ====================

    printf("Initializing ST7789v display...\n");
    if (!ST7789V_Init(&display_config))
    {
        printf("ERROR: Failed to initialize ST7789v display!\n");
        while (1)
            tight_loop_contents();
    }

    printf("ST7789v display initialized successfully!\n");

    // Get usable display dimensions
    uint16_t usable_width, usable_height;
    ST7789V_GetUsableDimensions(&usable_width, &usable_height);
    printf("Usable display area: %dx%d pixels\n", usable_width, usable_height);

    // ==================== Main Display Loop ====================

    while (1)
    {
        printf("\n=== ST7789v Library Demo Cycle ===\n");

        // -------------------- Screen Clearing --------------------
        printf("Step 1: Clearing screen to black...\n");
        ST7789V_ClearScreen(ST7789V_COLOR_BLACK);
        sleep_ms(2000);

        // -------------------- Colored Rectangles --------------------
        printf("Step 2: Drawing colored rectangles...\n");

        // Use logical coordinates (0-based) - library handles translation
        ST7789V_DrawRectangle(5, 5, 15, 15, ST7789V_COLOR_RED);    // Red square
        ST7789V_DrawRectangle(25, 5, 15, 15, ST7789V_COLOR_GREEN); // Green square
        ST7789V_DrawRectangle(45, 5, 15, 15, ST7789V_COLOR_BLUE);  // Blue square

        ST7789V_DrawRectangle(5, 25, 15, 15, ST7789V_COLOR_YELLOW);   // Yellow square
        ST7789V_DrawRectangle(25, 25, 15, 15, ST7789V_COLOR_CYAN);    // Cyan square
        ST7789V_DrawRectangle(45, 25, 15, 15, ST7789V_COLOR_MAGENTA); // Magenta square

        ST7789V_DrawRectangle(5, 45, 55, 10, ST7789V_COLOR_WHITE); // White bar

        printf("Colored rectangles drawn using logical coordinates.\n");
        sleep_ms(3000);

        // -------------------- Text Rendering --------------------
        printf("Step 3: Rendering text...\n");

        // Draw text using library functions
        ST7789V_DrawText("HELLO", 10, 70, ST7789V_COLOR_WHITE);
        ST7789V_DrawText("ST7789V", 10, 85, ST7789V_COLOR_CYAN);
        ST7789V_DrawText("LIBRARY", 10, 100, ST7789V_COLOR_YELLOW);

        // Draw additional text with different colors
        ST7789V_DrawText("PICO", 10, 120, ST7789V_COLOR_GREEN);
        ST7789V_DrawText("2025", 10, 135, ST7789V_COLOR_MAGENTA);

        printf("Text rendering complete.\n");
        sleep_ms(5000);

        // -------------------- Pixel Art Demo --------------------
        printf("Step 4: Drawing pixel art pattern...\n");

        // Create a simple pixel art pattern using individual pixels
        for (int y = 160; y < 180; y++)
        {
            for (int x = 10; x < 50; x++)
            {
                uint16_t color;

                // Create a gradient pattern
                if ((x + y) % 4 == 0)
                    color = ST7789V_COLOR_RED;
                else if ((x + y) % 4 == 1)
                    color = ST7789V_COLOR_GREEN;
                else if ((x + y) % 4 == 2)
                    color = ST7789V_COLOR_BLUE;
                else
                    color = ST7789V_COLOR_WHITE;

                ST7789V_DrawPixel(x, y, color);
            }
        }

        printf("Pixel art pattern complete.\n");
        sleep_ms(3000);

        // -------------------- Status Display --------------------
        printf("Step 5: Displaying status information...\n");

        ST7789V_DrawText("STATUS:", 5, 190, ST7789V_COLOR_WHITE);
        ST7789V_DrawText("OK", 45, 190, ST7789V_COLOR_GREEN);

        char dimension_text[20];
        snprintf(dimension_text, sizeof(dimension_text), "%dX%d", usable_width, usable_height);
        ST7789V_DrawText(dimension_text, 5, 205, ST7789V_COLOR_CYAN);

        printf("Status display complete.\n");
        printf("=== Demo cycle finished - restarting in 5 seconds ===\n");

        sleep_ms(5000);
    }
}