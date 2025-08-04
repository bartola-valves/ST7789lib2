#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "lib/st7789.h"
#include "lib/gfx.h"

// GPIO16 and GPIO17 are used for UART in this program

// lcd configuration

const int lcd_width = 170;  // Width of the display
const int lcd_height = 320; // Height of the display

int main()
{
    int rotation_test = 0;
    stdio_init_all();

    // Add delay to ensure UART is ready
    sleep_ms(2000);
    printf("Starting ST7789 test with smaller font...\n");

    // create a new instance of the SPI peripheral
    spi_inst_t *st7789_spi = spi0;
    printf("SPI peripheral set to spi0\n");

    // set the pins for the SPI peripheral
    LCD_setPins(4, 1, 5, 2, 3);
    printf("Pins set: DC=4, CS=1, RST=5, SCK=2, TX=3\n");

    // set the SPI peripheral
    LCD_setSPIperiph(st7789_spi);
    printf("SPI peripheral configured\n");

    printf("Initializing display...\n");
    LCD_initDisplay(lcd_width, lcd_height);
    printf("Display initialized\n");

    // Fix the upside down issue - rotate 180 degrees
    LCD_setRotation(2); // 180 degrees rotation to flip it right-side up
    printf("Rotation set to 2 (180 degrees)\n");

    printf("Creating framebuffer...\n");
    GFX_createFramebuf();
    printf("Framebuffer created\n");

    printf("Testing with smaller font...\n");
    int c = 0;
    while (1)
    {
        // Clear screen first
        GFX_fillScreen(0x0000);

        // Set smaller text size - try size 1 instead of default 4
        GFX_setTextSize(1);

        // Set text color
        GFX_setTextColor(0xFFFF);

        // Text at top
        GFX_setCursor(5, 5);
        GFX_printf("Frame: %d", c++);

        GFX_setCursor(5, 15); // Small spacing since font is smaller
        GFX_printf("Size: %dx%d", lcd_width, lcd_height);

        GFX_setCursor(5, 25);
        GFX_printf("Font size: 1");

        // Draw colored rectangles with good spacing
        GFX_fillRect(10, 40, 40, 12, 0xF800); // Red rectangle
        GFX_fillRect(10, 55, 40, 12, 0x07E0); // Green rectangle
        GFX_fillRect(10, 70, 40, 12, 0x001F); // Blue rectangle

        // Add labels
        GFX_setCursor(60, 42);
        GFX_printf("Red");
        GFX_setCursor(60, 57);
        GFX_printf("Green");
        GFX_setCursor(60, 72);
        GFX_printf("Blue");

        // Draw border to see bounds
        GFX_drawRect(0, 0, lcd_width - 1, lcd_height - 1, 0x07E0);

        GFX_flush();
        printf("Frame %d rendered\n", c - 1);
        sleep_ms(1000);
    }
}