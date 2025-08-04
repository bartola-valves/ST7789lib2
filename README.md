# ST7789 Display Library for Raspberry Pi Pico

A comprehensive library for controlling ST7789 TFT LCD displays with the Raspberry Pi Pico microcontroller. This library provides both low-level hardware control and high-level graphics functions with framebuffer support.

Developed for Raspberry Pico or RP2040 boards using the Pico SDK
@bartola-valves valves@bartola.co.uk

## Features

- ✅ **Hardware SPI interface** for fast communication
- ✅ **Framebuffer support** for smooth graphics operations
- ✅ **Multiple display sizes** with automatic offset correction
- ✅ **Text rendering** with variable font sizes
- ✅ **Shape drawing** (lines, rectangles, circles)
- ✅ **Color support** (16-bit RGB565)
- ✅ **Display rotation** (0°, 90°, 180°, 270°)
- ✅ **printf-style text output**

## Supported Display Sizes

- 170x320 pixels (tested and optimized)
- 172x320 pixels (1.47" displays)
- 240x280 pixels (1.69" displays)  
- 135x240 pixels (1.14" displays)
- 240x320 pixels (standard ST7789)

## Hardware Requirements

### Display Connections

| Display Pin | Pico GPIO | Function |
|-------------|-----------|----------|
| VCC         | 3.3V      | Power supply |
| GND         | GND       | Ground |
| SCL/SCK     | GPIO2     | SPI Clock |
| SDA/MOSI    | GPIO3     | SPI Data |
| CS          | GPIO1     | Chip Select |
| DC          | GPIO4     | Data/Command |
| RST         | GPIO5     | Reset |
| BLK         | 3.3V      | Backlight (connect to 3.3V) |

### UART Debug (Optional)
- TX: GPIO16
- RX: GPIO17
- Baud: 115200

## Quick Start

### 1. Basic Setup

```cpp
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "lib/st7789.h"
#include "lib/gfx.h"

int main() {
    // Initialize UART for debugging
    stdio_init_all();
    sleep_ms(2000);
    
    // Configure SPI and pins
    spi_inst_t *spi = spi0;
    LCD_setPins(4, 1, 5, 2, 3);  // DC, CS, RST, SCK, TX
    LCD_setSPIperiph(spi);
    
    // Initialize display
    LCD_initDisplay(170, 320);  // Width, Height
    LCD_setRotation(2);         // 180° rotation (adjust as needed)
    
    // Create framebuffer
    GFX_createFramebuf();
    
    // Your drawing code here...
    
    return 0;
}
```

### 2. Drawing Text

```cpp
// Set text properties
GFX_setTextSize(2);           // 2x scaling
GFX_setTextColor(0xFFFF);     // White text
GFX_setCursor(10, 10);        // Position cursor

// Draw text
GFX_printf("Hello World!");
GFX_flush();                  // Update display
```

### 3. Drawing Shapes

```cpp
// Fill screen with black
GFX_fillScreen(ST77XX_BLACK);

// Draw colored rectangles
GFX_fillRect(10, 50, 60, 30, ST77XX_RED);
GFX_fillRect(10, 90, 60, 30, ST77XX_GREEN);
GFX_fillRect(10, 130, 60, 30, ST77XX_BLUE);

// Draw circle
GFX_drawCircle(100, 100, 20, ST77XX_YELLOW);

// Draw border
GFX_drawRect(0, 0, 170, 320, ST77XX_WHITE);

// Update display
GFX_flush();
```

## API Reference

### LCD Functions (st7789.h)

#### Initialization
```cpp
void LCD_setPins(uint16_t dc, uint16_t cs, int16_t rst, uint16_t sck, uint16_t tx);
void LCD_setSPIperiph(spi_inst_t *s);
void LCD_initDisplay(uint16_t width, uint16_t height);
```

#### Display Control
```cpp
void LCD_setRotation(uint8_t m);  // 0=0°, 1=90°, 2=180°, 3=270°
void LCD_WritePixel(int x, int y, uint16_t col);
void LCD_WriteBitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *bitmap);
```

### GFX Functions (gfx.h)

#### Framebuffer Management
```cpp
void GFX_createFramebuf();    // Must call before drawing
void GFX_destroyFramebuf();
void GFX_flush();             // Update display with framebuffer
```

#### Text Functions
```cpp
void GFX_setCursor(int16_t x, int16_t y);
void GFX_setTextColor(uint16_t color);
void GFX_setTextSize(uint8_t size);
void GFX_printf(const char *format, ...);
```

#### Drawing Functions
```cpp
void GFX_drawPixel(int16_t x, int16_t y, uint16_t color);
void GFX_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void GFX_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void GFX_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void GFX_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void GFX_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void GFX_fillScreen(uint16_t color);
```

#### Bitmap Functions
```cpp
void GFX_drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, 
                    int16_t w, int16_t h, uint16_t color, uint16_t bg);
void GFX_drawBitmapMask(int16_t x, int16_t y, const uint8_t *bitmap, 
                        int16_t w, int16_t h, uint16_t color);
```

## Logo Support

This library includes support for displaying custom monochrome bitmaps, such as logos. A complete example is provided with the Bartola logo integration.

### Adding Your Logo

1. **Convert your image** to monochrome bitmap format (see [LOGO_CONVERSION.md](LOGO_CONVERSION.md))
2. **Create a header file** with your bitmap data:

```cpp
// my_logo.h
#define MY_LOGO_WIDTH  100
#define MY_LOGO_HEIGHT 50

const unsigned char my_logo_bitmap[] = {
    0x00, 0x00, 0x1F, 0xFF, 0x80,  // Your bitmap data here
    // ... more data
};
```

3. **Draw your logo**:

```cpp
#include "my_logo.h"

// Draw logo with transparent background
GFX_drawBitmapMask(10, 10, my_logo_bitmap, MY_LOGO_WIDTH, MY_LOGO_HEIGHT, ST77XX_WHITE);

// Or with solid background
GFX_drawBitmap(10, 10, my_logo_bitmap, MY_LOGO_WIDTH, MY_LOGO_HEIGHT, 
               ST77XX_WHITE, ST77XX_BLACK);
```

### Bartola Logo Example

The library includes a complete example showing how to integrate a custom logo:

- `bartola_logo_bitmap.h` - Logo bitmap data and constants
- `drawBartolaLogo()` and `drawBartolaLogoScaled()` functions in main program
- See [LOGO_CONVERSION.md](LOGO_CONVERSION.md) for conversion instructions

## Predefined Colors

Use these predefined color constants:

```cpp
ST77XX_BLACK     // 0x0000
ST77XX_WHITE     // 0xFFFF
ST77XX_RED       // 0xF800
ST77XX_GREEN     // 0x07E0
ST77XX_BLUE      // 0x001F
ST77XX_CYAN      // 0x07FF
ST77XX_MAGENTA   // 0xF81F
ST77XX_YELLOW    // 0xFFE0
ST77XX_ORANGE    // 0xFC00
```

Or create custom colors:
```cpp
uint16_t purple = GFX_RGB565(128, 0, 128);  // RGB to RGB565
```

## Build Configuration

### CMakeLists.txt
```cmake
add_executable(your_project main.cpp lib/st7789.cpp lib/gfx.cpp)

target_include_directories(your_project PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/lib
)

target_link_libraries(your_project
    pico_stdlib
    hardware_spi
    hardware_dma
)

# UART configuration (optional)
target_compile_definitions(your_project PRIVATE
    PICO_DEFAULT_UART_TX_PIN=16
    PICO_DEFAULT_UART_RX_PIN=17
)
```

## Troubleshooting

### Display Issues

**Blank Screen:**
- Check BLK/Backlight pin is connected to 3.3V
- Verify all connections match the pinout table
- Ensure proper power supply (3.3V)

**Upside Down Display:**
- Try different rotation values (0, 1, 2, 3)
- Most common fix: `LCD_setRotation(2)` for 180° flip

**Offset/Clipped Content:**
- The library automatically handles 170x320 display offsets
- For other sizes, check the offset calculations in `LCD_initDisplay()`

**Text Too Large:**
- Use `GFX_setTextSize(1)` for smallest text
- Size 2 is good for readability on 170x320 displays

### Build Issues

**Missing Headers:**
- Ensure `lib` directory is in include paths
- Check CMakeLists.txt configuration

**Link Errors:**
- Verify all required libraries are linked
- Check pico-sdk installation

## Example Projects

### Simple Text Display
```cpp
#include "pico/stdlib.h"
#include "lib/st7789.h"
#include "lib/gfx.h"

int main() {
    stdio_init_all();
    sleep_ms(2000);
    
    spi_inst_t *spi = spi0;
    LCD_setPins(4, 1, 5, 2, 3);
    LCD_setSPIperiph(spi);
    LCD_initDisplay(170, 320);
    LCD_setRotation(2);
    
    GFX_createFramebuf();
    GFX_setTextSize(2);
    GFX_setTextColor(ST77XX_WHITE);
    
    int counter = 0;
    while (1) {
        GFX_fillScreen(ST77XX_BLACK);
        GFX_setCursor(10, 10);
        GFX_printf("Counter: %d", counter++);
        GFX_flush();
        sleep_ms(1000);
    }
}
```

## License

This library is based on Adafruit's GFX library and modified for Raspberry Pi Pico.

## Contributing

Feel free to submit issues and enhancement requests!

## Changelog

### v1.0 (2025-01-XX)
- Initial release
- Support for 170x320 displays with automatic offset correction
- Framebuffer-based graphics system
- Complete text rendering with variable font sizes
- Shape drawing functions
- Hardware SPI optimization
