import sys
import struct

import tkinter as tk

from tkinter import filedialog

from SpriteMaker.constants import PALETTE_COLORS, FULL_BLOCK_CHAR_INT, TRANSPARENT_CHAR_INT, DEFAULT_COLOR_INT, \
    SPRITE_MAKER_VERSION
from WorldEditor.models import SpriteData

TRANSPARENT_CHAR_LE = int.from_bytes(bytes.fromhex("200e"), "little")
TRANSPARENT_CHAR_BE = int.from_bytes(bytes.fromhex("200e"), "big")


def is_transparent(char_value: int) -> bool:
    return char_value in (TRANSPARENT_CHAR_LE, TRANSPARENT_CHAR_BE)


def open_sprite_file() -> tuple[int, list[list[SpriteData]], str] | None:
    root = tk.Tk()
    root.withdraw()
    file_path = filedialog.askopenfilename(filetypes=[("CXSP Sprite", "*.cxsp")])

    if file_path:
        width, height, sprite_id, sprite_data = load_sprite(file_path)
        return sprite_id, sprite_data, file_path

    return None


def export_sprite(sprite_width: int, sprite_height: int,
                  colors: list[list[int]], export_path: str, sprite_id: int):
    """
    @brief Exports a sprite to a .cxsp file using struct.pack, with 32-bit header fields.

    File structure:
    - 4 bytes: version        (uint32)
    - 4 bytes: sprite_width   (uint32)
    - 4 bytes: sprite_height  (uint32)
    - 4 bytes: sprite_id      (uint32)
    - Per-tile: 2 bytes char + 2 bytes color attribute (uint16 + uint16)

    @param sprite_width The width of the sprite in tiles (1–4294967295).
    @param sprite_height The height of the sprite in tiles (1–4294967295).
    @param colors A 2D list [height][width] of color RGB tuples. Use None for transparent tiles.
    @param export_path The output path for the .cxsp file.
    @param sprite_id A unique 32-bit unsigned integer ID for this sprite.
    """
    if not (0 <= sprite_id <= 0xFFFFFFFF):
        raise ValueError("sprite_id must be a 32-bit unsigned integer (0–4294967295)")
    if not (0 < sprite_width <= 0xFFFFFFFF):
        raise ValueError("sprite_width must be a 32-bit unsigned integer (1–4294967295)")
    if not (0 < sprite_height <= 0xFFFFFFFF):
        raise ValueError("sprite_height must be a 32-bit unsigned integer (1–4294967295)")

    with open(export_path, 'wb') as f:
        # Write 32-bit header
        f.write(struct.pack('<IIII', SPRITE_MAKER_VERSION, sprite_width, sprite_height, sprite_id))

        for y in range(sprite_height):
            for x in range(sprite_width):
                color = colors[y][x]
                if color is None:
                    char = TRANSPARENT_CHAR_INT
                    attr = DEFAULT_COLOR_INT
                else:
                    color_index = PALETTE_COLORS.index(color)
                    char = FULL_BLOCK_CHAR_INT
                    attr = color_index
                f.write(struct.pack('<HH', char, attr))
        print(f'saved: {export_path}')


def load_sprite(file_path: str) -> tuple[int, int, int, list[list[SpriteData]]]:
    """
    Loads a .cxsp sprite file and extracts:
      - width (uint32)
      - height (uint32)
      - sprite_id (uint32)
      - color data as a 2D list (height x width)

    Assumes each tile in the sprite is a CHAR_INFO-like struct:
    - 2 bytes for the character (ignored)
    - 2 bytes for the color attribute (used)
    """
    if not file_path.endswith('.cxsp'):
        raise ValueError(f"Unsupported file type {file_path.split('.')[-1]}")

    with open(file_path, 'rb') as binary_sprite:
        version = int.from_bytes(binary_sprite.read(4), byteorder=sys.byteorder)
        width = int.from_bytes(binary_sprite.read(4), byteorder=sys.byteorder)
        height = int.from_bytes(binary_sprite.read(4), byteorder=sys.byteorder)
        sprite_id = int.from_bytes(binary_sprite.read(4), byteorder=sys.byteorder)

        sprite_data = []
        for y in range(height):
            row = []
            for x in range(width):
                char_bytes = binary_sprite.read(2)  # Ignored
                color_bytes = binary_sprite.read(2)
                char_value = int.from_bytes(char_bytes, byteorder=sys.byteorder)
                color_value = int.from_bytes(color_bytes, byteorder=sys.byteorder)

                if is_transparent(char_value):
                    char_value = -1
                else:
                    char_value = 1

                row.append(SpriteData(char_value, color_value))
            sprite_data.append(row)

    return width, height, sprite_id, sprite_data
