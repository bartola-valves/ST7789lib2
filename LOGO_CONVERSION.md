# Logo Bitmap Conversion Guide

## Converting Your Logo to Bitmap Format

The current `bartola_logo.h` file contains JPEG data, but our display library needs monochrome bitmap data. Here's how to convert your logo:

### Method 1: Using the Included Python Script (Recommended)

A Python conversion script is included for easy conversion:

```bash
# Install required dependency
pip install Pillow

# Convert your image (will auto-resize to fit display)
python convert_image.py your_logo.jpg bartola_logo 170 207

# This creates:
# - bartola_logo_bitmap.h (C header file)
# - bartola_logo_preview.png (preview of conversion)
```

The script automatically:
- Resizes your image to fit the display
- Converts to monochrome (1-bit) format
- Generates proper C array format
- Creates a preview image
- Provides usage instructions

### Method 2: Using Online Tools

1. **Convert JPEG to PNG/BMP**: 
   - Upload your logo to an online converter like [convertio.co](https://convertio.co/jpg-bmp/)
   - Convert to BMP or PNG format

2. **Convert to Monochrome**:
   - Use [photopea.com](https://photopea.com) (free online Photoshop alternative)
   - Open your image
   - Go to Image → Mode → Grayscale
   - Then Image → Mode → Bitmap
   - Choose "50% Threshold" for conversion method
   - Resize to fit display if needed (max 170 pixels wide)

3. **Convert to C Array**:
   - Use [javl.github.io/image2cpp](https://javl.github.io/image2cpp/)
   - Upload your monochrome bitmap
   - Set these options:
     - Canvas size: Use image size
     - Background color: Black
     - Invert image colors: Depends on your logo (try both)
     - Brightness/Threshold: Adjust as needed
   - Copy the generated C array

### Method 2: Using GIMP (Free Desktop Software)

1. Open your JPEG in GIMP
2. Go to Image → Mode → Grayscale
3. Go to Image → Mode → Indexed
4. Choose "Use black and white (1-bit) palette"
5. Scale image if needed (Image → Scale Image)
6. Export as XBM format (File → Export As → choose .xbm)
7. The XBM file contains C array data you can copy

### Method 3: Using ImageMagick (Command Line)

```bash
# Convert JPEG to monochrome bitmap
convert your_logo.jpg -resize 170x207 -monochrome -depth 1 logo.xbm

# The .xbm file will contain the C array data
```

## Integrating Your Bitmap

Once you have the bitmap array:

1. **Replace the placeholder data** in `bartola_logo_bitmap.h`:
   ```c
   const unsigned char bartola_logo_bitmap[] = {
       // Replace with your actual bitmap data
       0x00, 0x00, 0x1F, 0xFF, 0x80, 0x00, 0x00, 0x00,
       // ... your bitmap data here
   };
   ```

2. **Update the dimensions** if needed:
   ```c
   #define BARTOLA_LOGO_WIDTH  170  // Your logo width
   #define BARTOLA_LOGO_HEIGHT 207  // Your logo height
   ```

3. **Enable bitmap drawing** in `ST7789lib2.cpp`:
   - Uncomment this line in the `drawBartolaLogo()` function:
   ```cpp
   GFX_drawBitmapMask(x, y, bartola_logo_bitmap, BARTOLA_LOGO_WIDTH, BARTOLA_LOGO_HEIGHT, color);
   ```
   - Comment out the placeholder rectangle and text

## Testing Your Logo

1. Compile the project: Run the "Compile Project" task
2. Flash to your Pico: Run the "Run Project" task
3. Your logo should appear on the display

## Troubleshooting

- **Logo appears inverted**: Try adding `!` before the bitmap data or use the inverted option in image2cpp
- **Logo is too large**: Resize the source image before conversion
- **Logo is unclear**: Adjust the threshold/brightness settings during conversion
- **Build errors**: Check that the array is properly formatted with commas and semicolon

## Example Workflow

Here's a complete example of the conversion process:

1. Start with your JPEG logo
2. Use photopea.com to convert to monochrome bitmap
3. Use image2cpp to generate C array
4. Copy array to `bartola_logo_bitmap.h`
5. Update dimensions in the header file
6. Enable bitmap drawing in the main program
7. Compile and test

The display will show your logo alongside the test graphics!
