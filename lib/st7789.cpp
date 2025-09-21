// modified by Ale Moglia / @bartola-valves valves@bartola.co.uk
// version 0.1 - STT789 display driver for Raspberry Pi Pico
// This library provides hardware-specific functions for controlling ST7789 TFT LCD displays
// via SPI interface on the Raspberry Pi Pico microcontroller.
// date: 4th August 2025
//

#include "st7789.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include <stdio.h>

// ST7789 Commands
#define ST77XX_SWRESET 0x01
#define ST77XX_SLPOUT 0x11
#define ST77XX_COLMOD 0x3A
#define ST77XX_MADCTL 0x36
#define ST77XX_CASET 0x2A
#define ST77XX_RASET 0x2B
#define ST77XX_RAMWR 0x2C
#define ST77XX_INVON 0x21
#define ST77XX_INVOFF 0x20
#define ST77XX_NORON 0x13
#define ST77XX_DISPON 0x29
#define ST_CMD_DELAY 0x80

// MADCTL bit definitions
#define ST77XX_MADCTL_MY 0x80  ///< Page addr order: Bottom to Top
#define ST77XX_MADCTL_MX 0x40  ///< Column addr order: Right to Left
#define ST77XX_MADCTL_MV 0x20  ///< Page/Column order: Reverse Mode (swap X/Y)
#define ST77XX_MADCTL_ML 0x10  ///< Line addr order: LCD Refresh Bottom to Top
#define ST77XX_MADCTL_RGB 0x00 ///< Red-Green-Blue pixel order
#define ST77XX_MADCTL_BGR 0x08 ///< Blue-Green-Red pixel order

uint16_t _colstart = 0, _rowstart = 0, _colstart2 = 0, _rowstart2 = 0;

uint8_t tabcolor;

uint16_t _width;  ///< Display width as modified by current rotation
uint16_t _height; ///< Display height as modified by current rotation

uint16_t windowWidth;
uint16_t windowHeight;

int16_t _xstart = 0; ///< Internal framebuffer X offset
int16_t _ystart = 0; ///< Internal framebuffer Y offset

uint8_t rotation;

spi_inst_t *st7789_spi = spi_default;

uint16_t st7789_pinCS = 17;
uint16_t st7789_pinDC = 16;
int16_t st7789_pinRST = -1;

uint16_t st7789_pinSCK = PICO_DEFAULT_SPI_SCK_PIN;
uint16_t st7789_pinTX = PICO_DEFAULT_SPI_TX_PIN;

// uint16_t st7789_pinRST;

static const uint8_t generic_st7789[] = {9, ST77XX_SWRESET, ST_CMD_DELAY, 150, ST77XX_SLPOUT, ST_CMD_DELAY, 10, ST77XX_COLMOD, 1 + ST_CMD_DELAY, 0x55, 10, ST77XX_MADCTL, 1, 0x08, ST77XX_CASET, 4, 0x00,
                                         0, 0,
                                         240, ST77XX_RASET, 4, 0x00,
                                         0, 320 >> 8,
                                         320 & 0xFF, ST77XX_INVON, ST_CMD_DELAY, 10,
                                         ST77XX_NORON, ST_CMD_DELAY, 10, ST77XX_DISPON, ST_CMD_DELAY, 10}; //    10 ms delay

/**
 * @brief Initialization sequence for 76x264 ST7789 displays
 * Custom configuration for small form factor displays
 */
static const uint8_t st7789_76x264[] = {
    10,                              //  10 commands in list:
    ST77XX_SWRESET, ST_CMD_DELAY,    //  1: Software reset, no args, w/delay
    150,                             //     150 ms delay
    ST77XX_SLPOUT, ST_CMD_DELAY,     //  2: Out of sleep mode, no args, w/delay
    120,                             //     120 ms delay
    ST77XX_COLMOD, 1 + ST_CMD_DELAY, //  3: Set color mode, 1 arg + delay:
    0x55,                            //     16-bit color (RGB565)
    10,                              //     10 ms delay
    ST77XX_MADCTL, 1,                //  4: Memory access control, 1 arg:
    0x00,                            //     Normal orientation (no rotation/flip)
    ST77XX_CASET, 4,                 //  5: Column addr set, 4 args:
    0x00, 0x00,                      //     XSTART = 0
    0x00, 0x4B,                      //     XEND = 75 (76-1, 0x4B = 75)
    ST77XX_RASET, 4,                 //  6: Row addr set, 4 args:
    0x00, 0x00,                      //     YSTART = 0
    0x01, 0x07,                      //     YEND = 263 (264-1, 0x107 = 263)
    ST77XX_INVON, 0,                 //  7: Display inversion ON (try inverted)
    ST77XX_NORON, ST_CMD_DELAY,      //  8: Normal display on, w/delay
    10,                              //     10 ms delay
    ST77XX_DISPON, ST_CMD_DELAY,     //  9: Main screen turn on, w/delay
    100,                             //     100 ms delay
    ST77XX_RAMWR, 0                  //  10: Memory write (start drawing)
};

#ifdef USE_DMA
#endif

#ifdef USE_DMA
uint dma_tx;
dma_channel_config dma_cfg;
void waitForDMA()
{

    dma_channel_wait_for_finish_blocking(dma_tx);
}
#endif

void LCD_setPins(uint16_t dc, uint16_t cs, int16_t rst, uint16_t sck, uint16_t tx)
{
    st7789_pinDC = dc;
    st7789_pinCS = cs;
    st7789_pinRST = rst;
    st7789_pinSCK = sck;
    st7789_pinTX = tx;
}

void LCD_setSPIperiph(spi_inst_t *s)
{
    st7789_spi = s;
}

void initSPI()
{
    spi_init(st7789_spi, 1000 * 1000);                                     // Reduced from 10MHz to 1MHz for breadboard stability
    spi_set_format(st7789_spi, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST); // Mode 0
    gpio_set_function(st7789_pinSCK, GPIO_FUNC_SPI);
    gpio_set_function(st7789_pinTX, GPIO_FUNC_SPI);

    gpio_init(st7789_pinCS);
    gpio_set_dir(st7789_pinCS, GPIO_OUT);
    gpio_put(st7789_pinCS, 1);

    gpio_init(st7789_pinDC);
    gpio_set_dir(st7789_pinDC, GPIO_OUT);
    gpio_put(st7789_pinDC, 1);

    if (st7789_pinRST != -1)
    {
        gpio_init(st7789_pinRST);
        gpio_set_dir(st7789_pinRST, GPIO_OUT);
        gpio_put(st7789_pinRST, 1);
    }

#ifdef USE_DMA
    dma_tx = dma_claim_unused_channel(true);
    dma_cfg = dma_channel_get_default_config(dma_tx);
    channel_config_set_transfer_data_size(&dma_cfg, DMA_SIZE_16);
    channel_config_set_dreq(&dma_cfg, spi_get_dreq(st7789_spi, true));
#endif
}

void ST7789_Reset()
{
    if (st7789_pinRST != -1)
    {
        gpio_put(st7789_pinRST, 0);
        sleep_ms(5);
        gpio_put(st7789_pinRST, 1);
    }
}

void ST7789_Select()
{
    gpio_put(st7789_pinCS, 0);
}

void ST7789_DeSelect()
{
    gpio_put(st7789_pinCS, 1);
}

void ST7789_RegCommand()
{
    gpio_put(st7789_pinDC, 0);
}

void ST7789_RegData()
{
    gpio_put(st7789_pinDC, 1);
}

void ST7789_WriteCommand(uint8_t cmd)
{
    ST7789_RegCommand();
    spi_set_format(st7789_spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST); // Mode 0
    spi_write_blocking(st7789_spi, &cmd, sizeof(cmd));
}

void ST7789_WriteData(const uint8_t *buff, size_t buff_size)
{
    ST7789_RegData();
    spi_set_format(st7789_spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST); // Mode 0
    spi_write_blocking(st7789_spi, buff, buff_size);
}

void ST7789_SendCommand(uint8_t commandByte, const uint8_t *dataBytes,
                        uint8_t numDataBytes)
{
    ST7789_Select();

    ST7789_WriteCommand(commandByte);
    ST7789_WriteData(dataBytes, numDataBytes);

    ST7789_DeSelect();
}

void ST7789_displayInit(const uint8_t *addr)
{

    uint8_t numCommands, cmd, numArgs;
    uint16_t ms;

    numCommands = *(addr++); // Number of commands to follow
    while (numCommands--)
    {                                // For each command...
        cmd = *(addr++);             // Read command
        numArgs = *(addr++);         // Number of args to follow
        ms = numArgs & ST_CMD_DELAY; // If hibit set, delay follows args
        numArgs &= ~ST_CMD_DELAY;    // Mask out delay bit
        ST7789_SendCommand(cmd, addr, numArgs);
        addr += numArgs;

        if (ms)
        {
            ms = *(addr++); // Read post-command delay time (ms)
            if (ms == 255)
                ms = 500; // If 255, delay for 500 ms
            sleep_ms(ms);
        }
    }
}

void LCD_setRotation(uint8_t m)
{
    uint8_t madctl = 0;

    rotation = m & 3; // can't be higher than 3

    switch (rotation)
    {
    case 0:
        madctl = ST77XX_MADCTL_MX | ST77XX_MADCTL_MY | ST77XX_MADCTL_RGB;
        _xstart = _colstart;
        _ystart = _rowstart;
        _width = windowWidth;
        _height = windowHeight;
        break;
    case 1:
        madctl = ST77XX_MADCTL_MY | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB;
        _xstart = _rowstart;
        _ystart = _colstart2;
        _height = windowWidth;
        _width = windowHeight;
        break;
    case 2:
        madctl = ST77XX_MADCTL_RGB;
        _xstart = _colstart2;
        _ystart = _rowstart2;
        _width = windowWidth;
        _height = windowHeight;
        break;
    case 3:
        madctl = ST77XX_MADCTL_MX | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB;
        _xstart = _rowstart2;
        _ystart = _colstart;
        _height = windowWidth;
        _width = windowHeight;
        break;
    }

    ST7789_SendCommand(ST77XX_MADCTL, &madctl, 1);
}

void LCD_initDisplay(uint16_t width, uint16_t height)
{

    initSPI();

    if (width == 172 && height == 320)
    {
        // 1.47" display
        _rowstart = _rowstart2 = 0;
        _colstart = _colstart2 = 34;
    }
    else if (width == 240 && height == 280)
    {
        // 1.69" display
        _rowstart = 20;
        _rowstart2 = 0;
        _colstart = _colstart2 = 0;
    }
    else if (width == 135 && height == 240)
    {
        // 1.14" display
        _rowstart = _rowstart2 = (int)((320 - height) / 2);
        // This is the only device currently supported device that has different
        // values for _colstart & _colstart2. You must ensure that the extra
        // pixel lands in _colstart and not in _colstart2
        _colstart = (int)((240 - width + 1) / 2);
        _colstart2 = (int)((240 - width) / 2);
    }
    else if (width == 170 && height == 320)
    {
        // 170x320 display - center the 170px width in 240px controller
        _rowstart = _rowstart2 = 0;  // No row offset needed
        _colstart = _colstart2 = 35; // Center horizontally: (240-170)/2 = 35
    }
    else if (width == 76 && height == 264)
    {
        // 76x264 display - ZERO offsets, raw addressing
        _rowstart = _rowstart2 = 0;
        _colstart = _colstart2 = 0;
        printf("76x264 display detected: Using ZERO offsets\n");
    }
    else
    {
        // 1.3", 1.54", and 2.0" displays
        _rowstart = (320 - height);
        _rowstart2 = 0;
        _colstart = _colstart2 = (240 - width);
    }

    windowWidth = width;
    windowHeight = height;
    ST7789_Select();
    ST7789_Reset();
    ST7789_displayInit(generic_st7789);

    // Fix inverted colors by turning OFF display inversion
    ST7789_SendCommand(ST77XX_INVOFF, NULL, 0); // Turn OFF inversion

    // LCD_setRotation(2);  // COMMENTED OUT - May be causing white noise issue
}

void LCD_setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    // RAW addressing - no offsets for 76x264 display
    uint32_t xa = ((uint32_t)x << 16) | (x + w - 1);
    uint32_t ya = ((uint32_t)y << 16) | (y + h - 1);

    xa = __builtin_bswap32(xa);
    ya = __builtin_bswap32(ya);

    ST7789_WriteCommand(ST77XX_CASET);
    ST7789_WriteData((uint8_t *)&xa, sizeof(xa));

    // row address set
    ST7789_WriteCommand(ST77XX_RASET);
    ST7789_WriteData((uint8_t *)&ya, sizeof(ya));

    // write to RAM
    ST7789_WriteCommand(ST77XX_RAMWR);
}

void LCD_WriteBitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
    ST7789_Select();
    LCD_setAddrWindow(x, y, w, h); // Clipped area
    ST7789_RegData();
    spi_set_format(st7789_spi, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST); // Mode 0
#ifdef USE_DMA
    dma_channel_configure(dma_tx, &dma_cfg,
                          &spi_get_hw(st7789_spi)->dr, // write address
                          bitmap,                      // read address
                          w * h,                       // element count (each element is of size transfer_data_size)
                          true);                       // start asap
    waitForDMA();
#else

    spi_write16_blocking(st7789_spi, bitmap, w * h);
#endif

    ST7789_DeSelect();
}

void LCD_WritePixel(int x, int y, uint16_t col)
{
    ST7789_Select();
    LCD_setAddrWindow(x, y, 1, 1); // Clipped area
    ST7789_RegData();
    spi_set_format(st7789_spi, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST); // Mode 0
    spi_write16_blocking(st7789_spi, &col, 1);
    ST7789_DeSelect();
}

/**
 * @brief Initialize 76x264 ST7789 display with optimized settings
 * @param width Display width (should be 76)
 * @param height Display height (should be 264)
 * @return true if successful, false if dimensions don't match
 */
bool LCD_init76x264Display(uint16_t width, uint16_t height)
{
    // Verify this is the correct display size
    if (width != 76 || height != 264)
    {
        printf("ERROR: LCD_init76x264Display called with wrong dimensions: %dx%d\n", width, height);
        return false;
    }

    printf("Initializing 76x264 ST7789 display...\n");

    // Initialize SPI
    initSPI();

    // Set up display parameters for 76x264 - SIMPLE APPROACH
    _width = width;
    _height = height;
    windowWidth = width;
    windowHeight = height;

    // CRITICAL: No offsets, no rotations - raw addressing
    _colstart = _colstart2 = 0;
    _rowstart = _rowstart2 = 0;
    _xstart = 0;
    _ystart = 0;

    printf("Display parameters set: %dx%d, zero offsets\n", width, height);

    // Select display
    ST7789_Select();
    printf("Display selected\n");

    // Reset display
    ST7789_Reset();
    printf("Display reset complete\n");

    // Send MINIMAL initialization - just the basics
    printf("Sending minimal initialization...\n");

    // Software reset
    ST7789_SendCommand(0x01, NULL, 0);
    sleep_ms(150);

    // Sleep out
    ST7789_SendCommand(0x11, NULL, 0);
    sleep_ms(120);

    // Color mode - 16-bit
    uint8_t colmod = 0x55;
    ST7789_SendCommand(0x3A, &colmod, 1);
    sleep_ms(10);

    // Display on
    ST7789_SendCommand(0x29, NULL, 0);
    sleep_ms(100);

    printf("Minimal 76x264 ST7789 display initialization complete\n");
    return true;
}