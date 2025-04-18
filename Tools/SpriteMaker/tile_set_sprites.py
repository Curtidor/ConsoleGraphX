import os
import sys
from PIL import Image

from utils import int16_to_hex_string, reverse_bytes_for_little_endian, write_hex_file

# Constants for CHAR_INFO-style layout
TRANSPARENT_CHAR = "20 0E"
FULL_BLOCK_CHAR = "25 88"
DEFAULT_COLOR = "00 00"


def build_palette(image: Image.Image, transparent_color=(0, 0, 0)) -> list[tuple[int, int, int]]:
    unique_colors = []
    for pixel in image.getdata():
        if pixel != transparent_color and pixel not in unique_colors:
            unique_colors.append(pixel)
    return unique_colors


def export_sprite(sprite_width: int, sprite_height: int,
                  colors: list[list[int]], export_path: str, sprite_id: int):
    if not (0 <= sprite_id <= 0xFFFF):
        raise ValueError("sprite_id must be a positive 16-bit value (0–65535)")
    if not (0 < sprite_width <= 0xFFFF) or not (0 < sprite_height <= 0xFFFF):
        raise ValueError("sprite dimensions must be positive 16-bit integers")

    output = []
    output.extend([int16_to_hex_string(sprite_width), "00 00", int16_to_hex_string(sprite_height), "00 00"])
    output.extend([int16_to_hex_string(sprite_id), "00 00"])

    for y in range(sprite_height):
        for x in range(sprite_width):
            color_index = colors[y][x]
            if color_index is None:
                output.append(TRANSPARENT_CHAR)
                output.append(DEFAULT_COLOR)
            else:
                output.append(FULL_BLOCK_CHAR)
                output.append(int16_to_hex_string(color_index))

    formatted = reverse_bytes_for_little_endian(output) if sys.byteorder == "little" else output
    byte_data = "".join(formatted).replace(" ", "")
    write_hex_file(export_path, byte_data)


def convert_tileset_with_transparency(image_path: str, tile_size: int, output_dir: str):
    image = Image.open(image_path).convert("RGBA")
    image_width, image_height = image.size

    tiles_x = image_width // tile_size
    tiles_y = image_height // tile_size

    palette = []
    sprite_id = 0

    for ty in range(tiles_y):
        for tx in range(tiles_x):
            sprite_colors = []
            for y in range(tile_size):
                row = []
                for x in range(tile_size):
                    pixel = image.getpixel((tx * tile_size + x, ty * tile_size + y))
                    r, g, b, a = pixel
                    if a == 0:
                        row.append(None)
                    else:
                        color = (r, g, b)
                        if color not in palette:
                            palette.append(color)
                        row.append(palette.index(color))
                sprite_colors.append(row)

            export_path = os.path.join(output_dir, f"sprite_{sprite_id}.cxsp")
            export_sprite(tile_size, tile_size, sprite_colors, export_path, sprite_id)
            sprite_id += 1

    print("Final palette:")
    for i, color in enumerate(palette):
        print(f"{i}: {color}")


def convert_tileset_to_sprites(image_path: str, tile_size: int, output_dir: str):
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    image = Image.open(image_path).convert("RGB")
    image_width, image_height = image.size
    palette = build_palette(image)

    tiles_x = image_width // tile_size
    tiles_y = image_height // tile_size

    print(f"Tileset size: {tiles_x} x {tiles_y}")
    print(f"Found {len(palette)} unique colors (excluding transparent).")
    print("Exporting tiles...")

    sprite_id = 0
    for ty in range(tiles_y):
        for tx in range(tiles_x):
            sprite_data = []
            for y in range(tile_size):
                row = []
                for x in range(tile_size):
                    pixel = image.getpixel((tx * tile_size + x, ty * tile_size + y))
                    if pixel == (0, 0, 0):
                        row.append(None)
                    else:
                        row.append(palette.index(pixel))
                sprite_data.append(row)

            export_path = os.path.join(output_dir, f"sprite_{sprite_id}.cxsp")
            export_sprite(tile_size, tile_size, sprite_data, export_path, sprite_id)
            print(f"Saved {export_path}")
            sprite_id += 1

    print("\nPalette Colors (RGB):")
    for i, color in enumerate(palette):
        print(f"{i}: {color}")

    print(f"\nDone! Exported {sprite_id} sprites.")


# === Entry point ===
if __name__ == "__main__":
    import tkinter as tk
    from tkinter import filedialog, simpledialog

    root = tk.Tk()
    root.withdraw()

    img_path = filedialog.askopenfilename(title="Select tileset image", filetypes=[("PNG files", "*.png")])
    if not img_path:
        sys.exit("No image selected.")

    tile_size = simpledialog.askinteger("Tile Size", "Enter tile size in pixels:", minvalue=1)
    if not tile_size:
        sys.exit("No tile size provided.")

    output_dir = filedialog.askdirectory(title="Select output folder")
    if not output_dir:
        sys.exit("No output folder selected.")

    convert_tileset_with_transparency(img_path, tile_size, output_dir)
