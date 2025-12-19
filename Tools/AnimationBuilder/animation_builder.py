import struct
from Config.settings import ANIMATION_MARKER_VERSION
from SpriteMaker.tile_set_sprites import get_tile_set_data


def save_animation(texture_id_to_frames: dict[int, list[int]], texture_id_to_path: dict[int, str],
                   output_path: str = "./Animations/player_idle.cxaim"):
    with open(output_path, 'wb') as f:
        total_frames = sum(len(frames) for frames in texture_id_to_frames.values())
        total_files = len(texture_id_to_path)

        # write header
        f.write(struct.pack('<III', ANIMATION_MARKER_VERSION, total_frames, total_files))

        # write path_length:path_str
        for index, (sprite_id, path) in enumerate(texture_id_to_path.items()):
            encoded_path = path.encode('utf-8')
            path_len = len(encoded_path)
            if path_len > 65535:
                raise ValueError(f"Path too long for sprite {sprite_id}: {path}")
            f.write(struct.pack('<H', path_len))
            f.write(encoded_path)

        # the idea here is texture_id_to_frames is a dict like so
        #   - the key is the texture id
        #   - the value is a list of every frame it appears in
        # for example:
        # {1: [1,3]}
        # {2: [2]}
        # this means texture_id 1 is in frames 1 and 3 and texture_id 2 is only frame 2
        # below we are mapping the frame number to the texture id in a flat way
        frame_to_texture_id = {frame_number: -1 for frame_number in range(total_frames)}
        for texture_id, frame_numbers in texture_id_to_frames.items():
            for frame_number in frame_numbers:
                frame_to_texture_id[frame_number-1] = texture_id

        # write animation frames
        for texture_id in frame_to_texture_id.values():
            duration_ms = 1000  # TODO, placeholder duration; replace with real timing if needed
            f.write(struct.pack('<II', texture_id, duration_ms))


def main():
    sprite_id_to_frames, sprite_id_to_path = get_tile_set_data()

    save_animation(sprite_id_to_frames, sprite_id_to_path)


if __name__ == "__main__":
    main()