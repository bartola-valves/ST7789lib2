#!/usr/bin/env python3
"""
Convert hex color data to monochrome bitmap for ST7789 display
"""

import re

def hex_to_bitmap(input_file, output_name="bartola_logo", width=170, height=207, threshold=128):
    """Convert hex color data to monochrome bitmap C array"""
    
    print(f"Converting {input_file} to monochrome bitmap...")
    
    try:
        # Read the hex color data
        with open(input_file, 'r') as f:
            content = f.read()
        
        # Extract all hex colors
        hex_colors = re.findall(r'#([0-9a-f]{6})', content.lower())
        
        if len(hex_colors) != width * height:
            print(f"❌ Error: Expected {width * height} pixels, found {len(hex_colors)}")
            return False
        
        print(f"✅ Found {len(hex_colors)} pixels ({width}x{height})")
        
        # Convert to monochrome bitmap data  
        bitmap_data = []
        for y in range(height):
            for x in range(0, width, 8):
                byte = 0
                for bit in range(8):
                    if x + bit < width:
                        pixel_index = y * width + (x + bit)
                        hex_color = hex_colors[pixel_index]
                        
                        # Convert hex to RGB and calculate brightness
                        r = int(hex_color[0:2], 16)
                        g = int(hex_color[2:4], 16) 
                        b = int(hex_color[4:6], 16)
                        brightness = (r + g + b) / 3
                        
                        # Set bit if pixel is dark (below threshold)
                        if brightness < threshold:
                            byte |= (1 << (7 - bit))
                
                bitmap_data.append(byte)
        
        # Generate C header content
        header_content = f"""#ifndef {output_name.upper()}_BITMAP_H
#define {output_name.upper()}_BITMAP_H

// {output_name.title()} Logo Bitmap Data
// Size: {width}x{height} pixels
// Format: Monochrome bitmap (1 bit per pixel)
// Generated from hex color data

#define {output_name.upper()}_WIDTH  {width}
#define {output_name.upper()}_HEIGHT {height}

// Monochrome bitmap data for {output_name} logo
// Each bit represents one pixel: 1 = foreground color, 0 = background
const unsigned char {output_name}_bitmap[] = {{
"""
        
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
void draw{output_name.title().replace('_', '')}Logo(int x, int y, uint16_t color);
void draw{output_name.title().replace('_', '')}LogoScaled(int x, int y, uint16_t color, int scale);

#endif // {output_name.upper()}_BITMAP_H"""
        
        # Write header file
        output_file = f"{output_name}_bitmap.h"
        with open(output_file, 'w') as f:
            f.write(header_content)
        
        print(f"✅ Conversion successful!")
        print(f"📄 Output file: {output_file}")
        print(f"📐 Bitmap size: {width}x{height} pixels")
        print(f"💾 Data size: {len(bitmap_data)} bytes")
        print(f"🎯 Threshold used: {threshold} (0=all black, 255=all white)")
        
        # Count set bits for statistics
        total_bits = sum(bin(byte).count('1') for byte in bitmap_data)
        print(f"📊 Logo pixels: {total_bits}/{width*height} ({total_bits/(width*height)*100:.1f}%)")
        
        return True
        
    except Exception as e:
        print(f"❌ Error: {e}")
        return False

if __name__ == "__main__":
    success = hex_to_bitmap("bartola_raw", "bartola_logo", 170, 207, 128)
    if success:
        print(f"\n🚀 Ready to use! Replace the placeholder bitmap in your C code with the generated data.")
