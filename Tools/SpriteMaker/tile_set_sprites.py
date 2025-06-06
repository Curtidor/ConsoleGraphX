import os
import sys

from PIL import Image

from sprite_utils import export_sprite


def build_palette(image: Image.Image) -> list[tuple[int, int, int]]:
    """Builds a list of unique (R, G, B) colors from a PIL RGBA image."""
    pixels = image.getdata()
    unique_colors = set()
    for pixel in pixels:
        r, g, b, a = pixel
        if a != 0:  # skip transparent
            unique_colors.add((r, g, b))  # no alpha
    return list(unique_colors)


def convert_tile_set_with_transparency(image_path: str, tile_size: int, output_dir: str):
    image = Image.open(image_path).convert("RGBA")
    image_width, image_height = image.size

    tiles_x = image_width // tile_size
    tiles_y = image_height // tile_size

    palette = build_palette(image)
    sprite_id = 0

    for ty in range(tiles_y):
        for tx in range(tiles_x):
            sprite_colors = []
            for y in range(tile_size):
                for x in range(tile_size):
                    pixel = image.getpixel((tx * tile_size + x, ty * tile_size + y))
                    r, g, b, a = pixel
                    if a == 0:
                        sprite_colors.append(None)
                    else:
                        color: tuple[int, int, int] = (r, g, b)
                        if color not in palette:
                            palette.append(color)
                        sprite_colors.append(color)

            export_path = os.path.join(output_dir, f"sprite_{sprite_id}.cxsp")
            export_sprite(tile_size, tile_size, sprite_colors, export_path, sprite_id, pallet=palette)
            sprite_id += 1

    print("Final palette:")
    for i, color in enumerate(palette):
        print(f"{i}: {color}")


def convert_tile_set_to_sprites(image_path: str, tile_size: int, output_dir: str):
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
    print(palette)

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
                        row.append(*pixel)
                sprite_data.append(row)

            export_path = os.path.join(output_dir, f"sprite_{sprite_id}.cxsp")
            export_sprite(tile_size, tile_size, sprite_data, export_path, sprite_id, pallet=palette)
            print(f"Saved {export_path}")
            sprite_id += 1

    print("\nPalette Colors (RGB):")
    for i, color in enumerate(palette):
        print(f"{i}: {color}")

    print(f"\nDone! Exported {sprite_id} sprites.")


def main():
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

    convert_tile_set_with_transparency(img_path, tile_size, output_dir)


if __name__ == "__main__":
    main()
