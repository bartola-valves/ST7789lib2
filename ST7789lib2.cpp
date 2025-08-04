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
    int c{0};
    stdio_init_all();

    // Add delay to ensure UART is ready
    sleep_ms(2000);
    printf("Starting ST7789 test...\n");

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
    // initialize the display with correct dimensions
    LCD_initDisplay(lcd_width, lcd_height);
    printf("Display initialized\n");

    // Back to working rotation
    LCD_setRotation(0);
    printf("Rotation set to 0\n");

    printf("Creating framebuffer...\n");
    // Create the framebuffer for GFX operations
    GFX_createFramebuf();
    printf("Framebuffer created\n");

    printf("Filling screen with black first...\n");
    // Fill entire screen with black to clear any garbage
    GFX_fillScreen(0x0000); // Black color
    GFX_flush();
    sleep_ms(100);

    printf("Setting text color...\n");
    GFX_setTextColor(0xFFFF); // White text

    printf("Starting main loop...\n");
    while (1)
    {
        // Clear screen first
        GFX_fillScreen(0x0000);

        // Set cursor to a safe position
        GFX_setCursor(5, 5);
        GFX_printf("Hello GFX!\nFrame: %d", c++);
        GFX_flush();
        printf("Frame %d rendered\n", c - 1);
        sleep_ms(1000);
    }
}