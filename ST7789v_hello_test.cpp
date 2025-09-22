/**
 * @file ST7789v_hello_test.cpp
 * @brief Copy of Working ST7789v_example.cpp - Step by Step
 * @author Ale Moglia
 * @date 2025
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "lib/st7789v.h"

int main()
{
    stdio_init_all();
    sleep_ms(2000);

    printf("Starting ST7789v Library Example...\n");
    printf("Display: 76×284 ST7789v TFT (284×76 rotated)\n");

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

        printf("Colored rectangles drawn using logical coordinates.\n");
        sleep_ms(3000);

        // -------------------- Text Rendering --------------------
        printf("Step 3: Rendering text...\n");

        // Draw text using library functions
        ST7789V_DrawText("HELLO", 10, 70, ST7789V_COLOR_WHITE);

        printf("Text rendering complete.\n");
        sleep_ms(5000);

        printf("=== Demo cycle finished - restarting in 5 seconds ===\n");
        sleep_ms(5000);
    }
}