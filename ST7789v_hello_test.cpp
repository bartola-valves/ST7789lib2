/**
 * @file ST7789v_hello_test.cpp
 * @brief EXACT copy of working ST7789v_example.cpp
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "lib/st7789v.h"

int main()
{
    stdio_init_all();
    sleep_ms(2000);

    printf("Starting ST7789v Library Example...\n");
    printf("Display: 76×264 ST7789v TFT (284×76 rotated)\n");

    ST7789V_Config_t display_config = {
        .spi_instance = spi1,
        .pin_dc = 8,
        .pin_cs = 9,
        .pin_rst = 6,
        .pin_sck = 10,
        .pin_mosi = 11,
        .pin_backlight = 12,
        .backlight_active_low = true,
        .color_inversion = false};

    printf("Initializing ST7789v display...\n");
    if (!ST7789V_Init(&display_config))
    {
        printf("ERROR: Failed to initialize ST7789v display!\n");
        while (1)
            tight_loop_contents();
    }

    printf("ST7789v display initialized successfully!\n");

    uint16_t usable_width, usable_height;
    ST7789V_GetUsableDimensions(&usable_width, &usable_height);
    printf("Usable display area: %dx%d pixels\n", usable_width, usable_height);

    while (1)
    {
        printf("\n=== ST7789v Library Demo Cycle ===\n");

        printf("Step 1: Clearing screen to black...\n");
        ST7789V_ClearScreen(ST7789V_COLOR_BLACK);
        sleep_ms(2000);

        printf("Step 2: Drawing colored rectangles...\n");
        ST7789V_DrawRectangle(5, 5, 15, 15, ST7789V_COLOR_RED);
        ST7789V_DrawRectangle(25, 5, 15, 15, ST7789V_COLOR_GREEN);
        ST7789V_DrawRectangle(45, 5, 15, 15, ST7789V_COLOR_BLUE);
        printf("Colored rectangles drawn using logical coordinates.\n");
        sleep_ms(3000);

        printf("Step 3: Rendering text...\n");
        ST7789V_DrawText("HELLO", 10, 70, ST7789V_COLOR_WHITE);
        printf("Text rendering complete.\n");
        sleep_ms(5000);

        printf("=== Demo cycle finished - restarting in 5 seconds ===\n");
        sleep_ms(5000);
    }
}