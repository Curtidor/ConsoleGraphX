import sys

import tkinter as tk
from dataclasses import dataclass

from tkinter import filedialog

from SpriteMaker.constants import PALETTE_COLORS, FULL_BLOCK_CHAR, TRANSPARENT_CHAR, DEFAULT_COLOR
from SpriteMaker.utils import int16_to_hex_string, reverse_bytes_for_little_endian, write_hex_file


@dataclass
class SpriteData:
    char: int
    color: int


TRANSPARENT_CHAR_LE = int.from_bytes(bytes.fromhex("200e"), "little")
TRANSPARENT_CHAR_BE = int.from_bytes(bytes.fromhex("200e"), "big")


def is_transparent(char_value: int) -> bool:
    return char_value in (TRANSPARENT_CHAR_LE, TRANSPARENT_CHAR_BE)


def open_sprite_file() -> tuple[int, list[list[SpriteData]]] | None:
    root = tk.Tk()
    root.withdraw()
    file_path = filedialog.askopenfilename(filetypes=[("CXSP Sprite", "*.cxsp")])

    if file_path:
        width, height, sprite_id, sprite_data = load_sprite(file_path)
        return sprite_id, sprite_data

    return None


def export_sprite(sprite_width: int, sprite_height: int,
                  colors: list[list[int]], export_path: str, sprite_id: int):
    """
    @brief Exports a sprite to a .cxsp file in a hex-based CHAR_INFO-like format.

    This function generates a sprite file (.cxsp) that stores character and color data
    in a format similar to the Windows CHAR_INFO structure. The file begins with a header
    containing the sprite width, height, and sprite ID, followed by pixel data where each
    tile consists of a 2-byte character and a 2-byte color attribute.

    The exported data is written as a hex string and saved using the provided export path.

    @param sprite_width The width of the sprite in tiles (must be a positive 16-bit integer).
    @param sprite_height The height of the sprite in tiles (must be a positive 16-bit integer).
    @param colors A 2D list [height][width] of color values. Use None for transparent tiles.
    @param export_path The file path to export the .cxsp sprite file (excluding extension).
    @param sprite_id A unique positive 16-bit ID for the sprite (0–65535).

    @throws ValueError if sprite_id, width, or height is out of the 16-bit positive range.
    """
    if not (0 <= sprite_id <= 0xFFFF):
        raise ValueError("sprite_id must be a positive 16-bit value (0–65535)")
    if not (0 < sprite_width <= 0xFFFF):
        raise ValueError("sprite_width must be a positive 16-bit value (1–65535)")
    if not (0 < sprite_height <= 0xFFFF):
        raise ValueError("sprite_height must be a positive 16-bit value (1–65535)")

    output = []
    output.extend([int16_to_hex_string(sprite_width), "00 00", int16_to_hex_string(sprite_height), "00 00"])
    output.extend([int16_to_hex_string(sprite_id), "00 00"])

    for y in range(sprite_height):
        row_output = []
        for x in range(sprite_width):
            color = colors[y][x]
            if color is None:
                row_output.append(TRANSPARENT_CHAR)
                row_output.append(DEFAULT_COLOR)
            else:
                color_index = PALETTE_COLORS.index(color)
                row_output.append(FULL_BLOCK_CHAR)
                row_output.append(int16_to_hex_string(color_index))
        output.extend(row_output)

    formatted_bytes = reverse_bytes_for_little_endian(output) if sys.byteorder == "little" else output
    byte_data = "".join(formatted_bytes).replace(" ", "")
    write_hex_file(export_path, byte_data)


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
