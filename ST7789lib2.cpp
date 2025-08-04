/**
 * @file ST7789lib2.cpp
 * @brief ST7789 TFT LCD Display Test Program
 * @author Ale Moglia
 * @date 2025
 *
 * Test program demonstrating the ST7789 display library functionality.
 * Displays text, colored rectangles, and a border on a 170x320 ST7789 TFT LCD.
 *
 * Hardware Configuration:
 * - Display: 170x320 ST7789 TFT LCD
 * - SPI: spi0 (GPIO2=SCK, GPIO3=MOSI)
 * - Control pins: DC=GPIO4, CS=GPIO1, RST=GPIO5
 * - Blank pin connected to 3.3V
 * - UART Debug: TX=GPIO16, RX=GPIO17 (115200 baud)
 * - Backlight: BLK pin connected to 3.3V
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "lib/st7789.h"
#include "lib/gfx.h"
#include "bartola_logo_bitmap.h"

// UART pins GPIO16 (TX) and GPIO17 (RX) are configured in CMakeLists.txt

/** @brief Display dimensions for 170x320 ST7789 LCD */
const int lcd_width = 170;  ///< Display width in pixels
const int lcd_height = 320; ///< Display height in pixels

/**
 * @brief Draw the Bartola logo at specified position
 * @param x X coordinate for top-left corner
 * @param y Y coordinate for top-left corner
 * @param color Color for logo foreground
 */
void drawBartolaLogo(int x, int y, uint16_t color)
{
    // Draw the actual Bartola logo bitmap with transparent background
    GFX_drawBitmapMask(x, y, bartola_logo_bitmap, BARTOLA_LOGO_WIDTH, BARTOLA_LOGO_HEIGHT, color);
}

/**
 * @brief Draw the Bartola logo with scaling
 * @param x X coordinate for top-left corner
 * @param y Y coordinate for top-left corner
 * @param color Color for logo foreground
 * @param scale Scaling factor (1=normal, 2=double size, etc.)
 */
void drawBartolaLogoScaled(int x, int y, uint16_t color, int scale)
{
    if (scale <= 1)
    {
        drawBartolaLogo(x, y, color);
        return;
    }

    // For now, scaled drawing just draws the normal logo
    // Implementing pixel-perfect scaling would require more complex code
    drawBartolaLogo(x, y, color);

    // Add a small indicator that this is scaled (for debugging)
    GFX_setCursor(x, y - 15);
    GFX_setTextSize(1);
    GFX_setTextColor(color);
    GFX_printf("x%d", scale);
    GFX_setTextSize(2); // Reset text size
}

/**
 * @brief Main program entry point
 *
 * Initializes the ST7789 display and demonstrates various graphics functions
 * including text rendering, shape drawing, and color display.
 *
 * @return int Program exit status (never reached due to infinite loop)
 */

int main()
{
    int rotation_test = 0;
    stdio_init_all();

    // Allow time for UART initialization and connection
    sleep_ms(2000);
    printf("Starting ST7789 test with optimized font size...\n");

    // Configure SPI peripheral instance
    spi_inst_t *st7789_spi = spi0;
    printf("SPI peripheral set to spi0\n");

    // Configure GPIO pins for ST7789 display
    // Parameters: DC, CS, RST, SCK, TX (MOSI)
    LCD_setPins(4, 1, 5, 2, 3);
    printf("Pins configured: DC=GPIO4, CS=GPIO1, RST=GPIO5, SCK=GPIO2, TX=GPIO3\n");

    // Assign SPI peripheral to LCD driver
    LCD_setSPIperiph(st7789_spi);
    printf("SPI peripheral configured for LCD\n");

    printf("Initializing display...\n");
    // Initialize display with automatic offset correction for 170x320 displays
    LCD_initDisplay(lcd_width, lcd_height);
    printf("Display initialized with %dx%d resolution\n", lcd_width, lcd_height);

    // Set display orientation - 180° rotation for correct viewing
    LCD_setRotation(2);
    printf("Display rotation set to 180 degrees\n");

    printf("Creating framebuffer...\n");
    // Allocate framebuffer memory for graphics operations
    GFX_createFramebuf();
    printf("Framebuffer created successfully\n");

    printf("Bartola logo loaded: %dx%d pixels (%d bytes)\n",
           BARTOLA_LOGO_WIDTH, BARTOLA_LOGO_HEIGHT,
           (BARTOLA_LOGO_WIDTH * BARTOLA_LOGO_HEIGHT + 7) / 8);

    printf("Starting graphics test loop...\n");
    int c = 0; ///< Frame counter for animation

    while (1)
    {
        // Clear framebuffer to black
        GFX_fillScreen(0x0000);

        // Configure text rendering - size 2 provides good readability
        GFX_setTextSize(2);

        // Set text color to white
        GFX_setTextColor(0xFFFF);

        // Display frame information at top of screen
        GFX_setCursor(5, 5);
        GFX_printf("Frame: %d", c++);

        // Display screen dimensions
        GFX_setCursor(5, 25);
        GFX_printf("Size: %dx%d", lcd_width, lcd_height);

        // Display current font size
        GFX_setCursor(5, 45);
        GFX_printf("Font size: 2");

        // Alternate between three display modes every 3 seconds
        int mode = (c / 3) % 3; // Switch every 3 frames, 3 modes total

        if (mode == 0)
        {
            // Mode 1: Basic rectangles and color demo
            GFX_setCursor(5, 65);
            GFX_printf("Mode: Basic");

            // Draw demonstration rectangles with primary colors
            GFX_fillRect(10, 85, 50, 15, 0xF800);  // Red rectangle
            GFX_fillRect(10, 105, 50, 15, 0x07E0); // Green rectangle
            GFX_fillRect(10, 125, 50, 15, 0x001F); // Blue rectangle

            // Add color labels next to rectangles
            GFX_setCursor(70, 87);
            GFX_printf("Red");
            GFX_setCursor(70, 107);
            GFX_printf("Green");
            GFX_setCursor(70, 127);
            GFX_printf("Blue");
        }
        else if (mode == 1)
        {
            // Mode 2: New features - custom colors and circles
            GFX_setCursor(5, 65);
            GFX_printf("Mode: New");

            // Demonstrate new color utility functions
            uint16_t custom_color = GFX_color565(255, 165, 0); // Orange color
            uint16_t purple = GFX_color565(128, 0, 128);       // Purple color
            uint16_t teal = GFX_color565(0, 128, 128);         // Teal color

            // Custom color rectangles
            GFX_fillRect(10, 85, 50, 15, custom_color);
            GFX_fillRect(10, 105, 50, 15, purple);
            GFX_fillRect(10, 125, 50, 15, teal);

            // Color labels - keep them shorter to fit
            GFX_setCursor(70, 87);
            GFX_printf("Orange");
            GFX_setCursor(70, 107);
            GFX_printf("Purple");
            GFX_setCursor(70, 127);
            GFX_printf("Teal");

            // Demonstrate circle drawing functions with plenty of space
            GFX_drawCircle(35, 155, 12, 0xFFFF);       // White circle outline
            GFX_fillCircle(85, 155, 10, 0xF81F);       // Magenta filled circle
            GFX_fillCircle(135, 155, 8, custom_color); // Orange filled circle

            // Circle labels
            GFX_setCursor(5, 175);
            GFX_setTextSize(1);
            GFX_printf("Outline   Filled   Custom");
            GFX_setTextSize(2); // Reset text size
        }
        else
        {
            // Mode 3: Logo-only mode - centered and properly displayed
            GFX_setCursor(5, 65);
            GFX_printf("Mode: Logo");

            // Center the logo vertically in the available space
            // Available space: from y=85 to y=315 (230 pixels)
            // Logo height: 207 pixels
            // Center position: 85 + (230-207)/2 = 85 + 11.5 ≈ 97
            drawBartolaLogo(0, 97, 0xFFFF); // White logo, centered
        }

        // // Draw Bartola logo in the lower area (only for modes 0 and 1)
        // if (mode != 2)
        // {
        //     // Position logo in center of display: (170-170)/2=0, start at y=190
        //     drawBartolaLogo(0, 190, 0xFFFF); // White logo
        // }

        // Draw border around entire display area for alignment verification
        GFX_drawRect(0, 0, lcd_width - 1, lcd_height - 1, 0x07E0);

        // Transfer framebuffer contents to physical display
        GFX_flush();

        // Log frame completion to UART with mode information
        const char *mode_names[] = {"Basic Colors", "New Features", "Logo Only"};
        printf("Frame %d rendered successfully (Mode: %s)\n", c - 1, mode_names[mode]);

        // Wait 1 second before next frame
        sleep_ms(1000);
    }
}