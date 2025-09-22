# ST7789v Display Library for Raspberry Pi Pico

A highly optimized display library for ST7789v variant TFT LCD displays, specifically tuned for 76×264 pixel displays commonly used in embedded applications.

## Features

- **Optimized for ST7789v**: Specifically designed for ST7789v display controller variant
- **Empirically Tuned**: Coordinate mappings discovered through real hardware testing
- **Complete Graphics Support**: Pixel drawing, rectangles, text rendering
- **Built-in Font System**: 5×7 bitmap font with A-Z, 0-9, and symbols
- **Hardware Optimization**: Direct SPI communication with proper CS/DC control
- **Color Management**: Automatic color inversion handling (INVOFF for ST7789v)
- **Coordinate Translation**: Automatic mapping from logical to display coordinates

## Hardware Specifications

| Parameter | Value | Description |
|-----------|-------|-------------|
| Physical Size | 76×264 pixels | Physical display dimensions |
| Rotated Size | 284×76 pixels | Dimensions after 90° rotation |
| Interface | MIPI SPI | Display communication protocol |
| Color Depth | 16-bit RGB565 | 65,536 colors |
| Width Offset | 18 pixels | Hardware coordinate offset |
| Height Offset | 82 pixels | Hardware coordinate offset |
| Working Range | 65×210 pixels | Usable display area |

## Pin Configuration

```cpp
ST7789V_Config_t config = {
    .spi_instance = spi1,           // SPI peripheral (spi0 or spi1)
    .pin_dc = 8,                    // Data/Command pin
    .pin_cs = 9,                    // Chip Select pin  
    .pin_rst = 6,                   // Reset pin
    .pin_sck = 10,                  // SPI Clock pin
    .pin_mosi = 11,                 // SPI MOSI pin
    .pin_backlight = 12,            // Backlight control (optional)
    .backlight_active_low = true,   // Backlight polarity
    .color_inversion = false        // Use INVOFF (recommended)
};
```

## Quick Start

### 1. Include the Library

```cpp
#include "lib/st7789v.h"
```

### 2. Initialize Display

```cpp
ST7789V_Config_t config = {
    // ... configure pins as above
};

if (!ST7789V_Init(&config)) {
    printf("Display initialization failed!\n");
    return -1;
}
```

### 3. Clear Screen

```cpp
ST7789V_ClearScreen(ST7789V_COLOR_BLACK);
```

### 4. Draw Graphics

```cpp
// Draw colored rectangles
ST7789V_DrawRectangle(10, 10, 20, 15, ST7789V_COLOR_RED);
ST7789V_DrawRectangle(35, 10, 20, 15, ST7789V_COLOR_GREEN);

// Draw individual pixels
ST7789V_DrawPixel(50, 50, ST7789V_COLOR_BLUE);
```

### 5. Render Text

```cpp
// Draw text strings
ST7789V_DrawText("HELLO", 10, 30, ST7789V_COLOR_WHITE);
ST7789V_DrawText("WORLD", 10, 45, ST7789V_COLOR_CYAN);

// Draw individual characters
ST7789V_DrawChar('A', 10, 60, ST7789V_COLOR_YELLOW);
```

## API Reference

### Initialization Functions

- `bool ST7789V_Init(const ST7789V_Config_t* config)` - Initialize display with configuration
- `void ST7789V_SetBacklight(bool enabled)` - Control backlight state
- `void ST7789V_SetColorInversion(bool inverted)` - Control color inversion

### Drawing Functions

- `void ST7789V_DrawPixel(uint16_t x, uint16_t y, uint16_t color)` - Draw single pixel
- `void ST7789V_DrawRawPixel(uint16_t x, uint16_t y, uint16_t color)` - Draw pixel with raw coordinates
- `void ST7789V_DrawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)` - Draw filled rectangle
- `void ST7789V_ClearScreen(uint16_t color)` - Clear entire display

### Text Rendering Functions

- `uint8_t ST7789V_DrawChar(char c, uint16_t x, uint16_t y, uint16_t color)` - Draw single character
- `uint16_t ST7789V_DrawText(const char* text, uint16_t x, uint16_t y, uint16_t color)` - Draw text string

### Utility Functions

- `void ST7789V_TranslateCoordinates(uint16_t logical_x, uint16_t logical_y, uint16_t* display_x, uint16_t* display_y)` - Convert logical to display coordinates
- `void ST7789V_GetUsableDimensions(uint16_t* width, uint16_t* height)` - Get usable display area

## Predefined Colors

| Color | Value | RGB565 |
|-------|-------|--------|
| `ST7789V_COLOR_BLACK` | 0x0000 | Black |
| `ST7789V_COLOR_WHITE` | 0xFFFF | White |
| `ST7789V_COLOR_RED` | 0xF800 | Red |
| `ST7789V_COLOR_GREEN` | 0x07E0 | Green |
| `ST7789V_COLOR_BLUE` | 0x001F | Blue |
| `ST7789V_COLOR_YELLOW` | 0xFFE0 | Yellow |
| `ST7789V_COLOR_CYAN` | 0x07FF | Cyan |
| `ST7789V_COLOR_MAGENTA` | 0xF81F | Magenta |

## Coordinate System

The library uses **logical coordinates** (0-based) that are automatically translated to the display's working coordinate range:

- **Logical coordinates**: (0,0) to (64,209) - What you specify in code
- **Display coordinates**: (60,40) to (125,250) - Actual hardware coordinates
- **Virtual coordinates**: (0,0) to (320,400) - Used for screen clearing

## Font System

Built-in 5×7 pixel bitmap font supports:
- **Uppercase letters**: A-Z
- **Digits**: 0-9  
- **Basic symbols**: Space, hyphen, underscore, period, colon, exclamation

Each character is 5 pixels wide, 7 pixels tall, with 1 pixel spacing (6 pixels total width).

## Build Instructions

Add to your CMakeLists.txt:

```cmake
add_executable(your_project
    your_main.cpp
    lib/st7789v.c
)

target_link_libraries(your_project
    pico_stdlib
    hardware_spi
    hardware_gpio
)
```

## Examples

See `ST7789v_example.cpp` for a comprehensive demonstration of all library features.

## Technical Notes

### Display Variants

This library is specifically optimized for ST7789v displays with:
- MIPI SPI interface
- 90° rotation configuration  
- Specific offset requirements (width=18, height=82)

### Performance Optimizations

- Direct SPI register access for maximum speed
- Efficient coordinate translation
- Optimized screen clearing algorithm
- Minimal memory footprint

### Color Inversion

ST7789v displays typically require **INVOFF** (color inversion disabled) for correct color display. This differs from standard ST7789 displays that may require INVON.

## License

MIT License - See LICENSE file for details.

## Author

Ale Moglia, 2025

## Changelog

### v1.0.0 (2025-01-XX)
- Initial release
- Complete ST7789v support with empirically discovered coordinate mappings
- Built-in bitmap font system
- Hardware-optimized SPI communication
- Comprehensive API with logical coordinate translation
