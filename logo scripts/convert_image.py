#!/usr/bin/env python3
"""
Image to C Bitmap Converter
Converts images to monochrome bitmaps suitable for ST7789 display library.

Requirements: pip install Pillow

Usage: python convert_image.py input_image.jpg [output_name] [width] [height]
"""

import sys
from PIL import Image

def convert_to_bitmap(input_path, output_name="logo", max_width=170, max_height=207):
    """Convert image to monochrome bitmap C array"""
    
    try:
        # Open and convert image
        img = Image.open(input_path)
        print(f"Original image size: {img.size}")
        
        # Resize if needed while maintaining aspect ratio
        img.thumbnail((max_width, max_height), Image.Resampling.LANCZOS)
        width, height = img.size
        print(f"Resized to: {width}x{height}")
        
        # Convert to grayscale then to 1-bit (monochrome)
        img = img.convert('L')  # Grayscale
        img = img.convert('1')  # 1-bit monochrome
        
        # Generate C header file
        header_content = f"""#ifndef {output_name.upper()}_BITMAP_H
#define {output_name.upper()}_BITMAP_H

// {output_name.title()} Logo Bitmap Data
// Size: {width}x{height} pixels
// Format: Monochrome bitmap (1 bit per pixel)

#define {output_name.upper()}_WIDTH  {width}
#define {output_name.upper()}_HEIGHT {height}

// Monochrome bitmap data for {output_name} logo
// Each bit represents one pixel: 1 = foreground color, 0 = background
const unsigned char {output_name}_bitmap[] = {{
"""
        
        # Convert image to bitmap array
        bitmap_data = []
        for y in range(height):
            for x in range(0, width, 8):
                byte = 0
                for bit in range(8):
                    if x + bit < width:
                        pixel = img.getpixel((x + bit, y))
                        if pixel == 0:  # Black pixel = set bit
                            byte |= (1 << (7 - bit))
                bitmap_data.append(byte)
        
        # Format bitmap data as C array
        bytes_per_line = 12
        for i, byte in enumerate(bitmap_data):
            if i % bytes_per_line == 0:
                header_content += "    "
            header_content += f"0x{byte:02X}"
            if i < len(bitmap_data) - 1:
                header_content += ", "
            if (i + 1) % bytes_per_line == 0:
                header_content += "\n"
        
        if len(bitmap_data) % bytes_per_line != 0:
            header_content += "\n"
        
        header_content += f"""}}; // Total bytes: {len(bitmap_data)}

// Function declarations for drawing the logo
void draw{output_name.title()}Logo(int x, int y, uint16_t color);
void draw{output_name.title()}LogoScaled(int x, int y, uint16_t color, int scale);

#endif // {output_name.upper()}_BITMAP_H"""
        
        # Write header file
        output_file = f"{output_name}_bitmap.h"
        with open(output_file, 'w') as f:
            f.write(header_content)
        
        print(f"✅ Converted successfully!")
        print(f"📄 Output file: {output_file}")
        print(f"📐 Bitmap size: {width}x{height} pixels")
        print(f"💾 Data size: {len(bitmap_data)} bytes")
        print(f"\nTo use in your code:")
        print(f"1. #include \"{output_file}\"")
        print(f"2. GFX_drawBitmapMask(x, y, {output_name}_bitmap, {output_name.upper()}_WIDTH, {output_name.upper()}_HEIGHT, color);")
        
        # Save preview image
        preview_file = f"{output_name}_preview.png"
        img.save(preview_file)
        print(f"🖼️  Preview saved: {preview_file}")
        
    except Exception as e:
        print(f"❌ Error: {e}")
        return False
    
    return True

def main():
    if len(sys.argv) < 2:
        print("Usage: python convert_image.py input_image.jpg [output_name] [width] [height]")
        print("Example: python convert_image.py logo.jpg bartola_logo 170 207")
        sys.exit(1)
    
    input_path = sys.argv[1]
    output_name = sys.argv[2] if len(sys.argv) > 2 else "logo"
    max_width = int(sys.argv[3]) if len(sys.argv) > 3 else 170
    max_height = int(sys.argv[4]) if len(sys.argv) > 4 else 207
    
    print(f"🔄 Converting {input_path} to C bitmap array...")
    convert_to_bitmap(input_path, output_name, max_width, max_height)

if __name__ == "__main__":
    main()
