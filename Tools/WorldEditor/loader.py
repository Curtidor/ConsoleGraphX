import struct
import os
import tkinter as tk
from tkinter import filedialog

from SpriteMaker.sprite_utils import load_sprite
from WorldEditor.models import PlacedSprite, SpriteEntry


def load_from_files(map_path: str, reg_path: str, tile_size: int):
    sprite_registry = {}
    with open(reg_path, "r") as reg_file:
        for line in reg_file:
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            if '=' in line:
                key, value = line.split('=', 1)
                sprite_id = int(key.strip())
                path = value.strip()
                sprite_registry[sprite_id] = path

    sprite_db = {}
    placed_sprites = []

    with open(map_path, "rb") as f:
        _version, = struct.unpack('<H', f.read(2))
        chunk_w, chunk_h = struct.unpack('<II', f.read(8))
        chunk_w *= tile_size
        chunk_h *= tile_size

        while True:
            count_data = f.read(4)
            if not count_data:
                break

            (sprite_count,) = struct.unpack('<I', count_data)
            if sprite_count == 0:
                continue

            for _ in range(sprite_count):
                x, y, sprite_id = struct.unpack('<iii', f.read(12))

                if sprite_id not in sprite_registry:
                    print(f"[WARN] Unknown sprite ID: {sprite_id}")
                    continue

                split_map_path = map_path.split('/')[:-1]
                full_path = '/'.join(split_map_path)
                sprite_path = full_path + '/' + sprite_registry[sprite_id]

                if sprite_path not in sprite_db:
                    _, _, _, sprite_data = load_sprite(sprite_path)
                    entry = SpriteEntry(sprite_id, sprite_data, sprite_path)
                    sprite_db[sprite_path] = entry
                else:
                    entry = sprite_db[sprite_path]

                grid_x = x // tile_size
                grid_y = y // tile_size

                placed_sprites.append(PlacedSprite(
                    position_x=x,
                    position_y=y,
                    grid_x=grid_x,
                    grid_y=grid_y,
                    sprite_entry=entry
                ))

            f.read(8)  # skip chunk terminator
    return chunk_w, chunk_h, placed_sprites, sprite_db


def load_map_and_registry(tile_size: int):
    root = tk.Tk()
    root.withdraw()
    root.attributes('-topmost', True)

    map_path = filedialog.askopenfilename(
        title="Select .cxmap file",
        filetypes=[("CX Map Files", "*.cxmap")],
    )
    if not map_path:
        print("Load cancelled.")
        return None

    reg_path = os.path.splitext(map_path)[0] + ".cxreg"
    if not os.path.exists(reg_path):
        print(f"Registry file not found: {reg_path}")
        return None

    print(f"[INFO] Loading map: {map_path}")
    print(f"[INFO] Loading registry: {reg_path}")

    return load_from_files(map_path, reg_path, tile_size)
