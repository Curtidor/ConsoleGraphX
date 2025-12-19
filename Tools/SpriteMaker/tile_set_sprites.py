import os
import sys
import hashlib
import tkinter as tk

from tkinter import filedialog, simpledialog
from PIL import Image, ImageTk

from Config.settings import SPRITE_ID
from SpriteMaker.sprite_utils import export_sprite


def build_palette(image: Image.Image) -> list[tuple[int, int, int]]:
    """Builds a list of unique (R, G, B) colors from a PIL RGBA image."""
    pixels = image.getdata()
    unique_colors = set()
    for pixel in pixels:
        r, g, b, a = pixel
        if a != 0:  # skip transparent
            unique_colors.add((r, g, b))  # no alpha
    return list(unique_colors)

# 15, 22


def display_sprite(sprite_colors, tile_width, tile_height):
    root = tk.Tk()
    root.title("Sprite Preview")

    img = Image.new("RGBA", (tile_width, tile_height))
    for y in range(tile_height):
        for x in range(tile_width):
            color = sprite_colors[y * tile_width + x]
            img.putpixel((x, y), color + (255,) if color else (0, 0, 0, 0))

    SCALE = 4
    scaled_img = img.resize((tile_width * SCALE, tile_height * SCALE), Image.NEAREST)

    tk_img = ImageTk.PhotoImage(scaled_img, master=root)
    label = tk.Label(root, image=tk_img)
    label.image = tk_img
    label.pack()

    input_frame = tk.Frame(root)
    input_frame.pack(pady=10)

    tk.Label(input_frame, text="File name (no extension):").grid(row=0, column=0, sticky="e")
    file_name_entry = tk.Entry(input_frame)
    file_name_entry.grid(row=0, column=1)

    tk.Label(input_frame, text="Sprite ID:").grid(row=1, column=0, sticky="e")
    texture_id_entry = tk.Entry(input_frame)
    texture_id_entry.grid(row=1, column=1)

    result = {"file_name": None, "texture_id": None}

    def on_submit():
        result["file_name"] = file_name_entry.get()
        try:
            result["texture_id"] = int(texture_id_entry.get())
            root.quit()  # End mainloop
        except ValueError:
            texture_id_entry.delete(0, tk.END)
            texture_id_entry.insert(0, "Invalid")

    submit_button = tk.Button(root, text="OK", command=on_submit)
    submit_button.pack(pady=5)

    root.mainloop()
    root.destroy()

    return result["file_name"], result["texture_id"]


def convert_tile_set_with_transparency(image_path: str, tile_width, tile_height: int,
                                       output_dir: str, enable_per_save=False, file_name='')\
        -> tuple[dict[int, list[int]], dict[int, str]]:
    image = Image.open(image_path).convert("RGBA")
    image_width, image_height = image.size

    tiles_x = image_width // tile_width
    tiles_y = image_height // tile_height

    palette = build_palette(image)
    texture_id = SPRITE_ID

    seen_hashes: dict[str, int] = {}
    # maps sprite id to a list of frame positions
    frame_positions: dict[int, list[int]] = {}
    id_to_path: dict[int, str] = {}
    for frame_position, (ty, tx) in enumerate(((y, x) for y in range(tiles_y) for x in range(tiles_x)), start=1):
        sprite_colors = []
        hash_data = bytearray()
        for y in range(tile_height):
            for x in range(tile_width):
                pixel = image.getpixel((tx * tile_width + x, ty * tile_height + y))
                r, g, b, a = pixel
                if a == 0:
                    sprite_colors.append(None)
                    hash_data.extend((0, 0, 0, 0))
                else:
                    color = (r, g, b)
                    if color not in palette:
                        palette.append(color)
                    sprite_colors.append(color)
                    hash_data.extend((*color, 255))  # RGBA

        hash_key = hashlib.md5(hash_data).hexdigest()
        if hash_key not in seen_hashes:
            seen_hashes[hash_key] = texture_id
            frame_positions[texture_id] = [frame_position]  # add the frame position
        else:
            idx = seen_hashes[hash_key]  # get the related sprite id
            frame_positions[idx].append(frame_position)
            print(f"[SKIPPED] Duplicate sprite at tile ({tx}, {ty})")
            continue

        if enable_per_save:
            user_file_name, texture_id_override = display_sprite(sprite_colors, tile_width, tile_height)
            if user_file_name is None or texture_id_override is None:
                print("Skipping this sprite.")
                continue
            texture_id = texture_id_override
        else:
            user_file_name = file_name  # fallback to static/default

        save_file_name = f'{user_file_name}_{texture_id}.cxsp' if user_file_name else f'sprite_{texture_id}.cxsp'
        export_path = os.path.join('Animations', save_file_name)

        export_sprite(tile_width, tile_height, sprite_colors, export_path, texture_id, pallet=palette)
        id_to_path[texture_id] = export_path
        texture_id += 1

    return frame_positions, id_to_path


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

    texture_id = 0
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

            export_path = os.path.join(output_dir, f"sprite_{texture_id}.cxsp")
            export_sprite(tile_size, tile_size, sprite_data, export_path, texture_id, pallet=palette)
            print(f"Saved {export_path}")
            texture_id += 1

    print("\nPalette Colors (RGB):")
    for i, color in enumerate(palette):
        print(f"{i}: {color}")

    print(f"\nDone! Exported {texture_id} sprites.")


def get_tile_set_data() -> tuple[dict[int, list[int]], dict[int, str]]:
    root = tk.Tk()
    root.withdraw()

    img_path = filedialog.askopenfilename(title="Select tileset image", filetypes=[("PNG files", "*.png")])
    if not img_path:
        sys.exit("No image selected.")

    tile_width = simpledialog.askinteger("Tile Width", "Enter tile Width in pixels:", minvalue=1)
    tile_height = simpledialog.askinteger("Tile Height", "Enter tile Height in pixels:", minvalue=1)
    if not tile_width or not tile_height:
        sys.exit("No tile size provided.")

    output_dir = filedialog.askdirectory(title="Select output folder")
    if not output_dir:
        sys.exit("No output folder selected.")

    return convert_tile_set_with_transparency(img_path, tile_width, tile_height, output_dir, enable_per_save=False, file_name='player_animation')


if __name__ == "__main__":
    get_tile_set_data()
