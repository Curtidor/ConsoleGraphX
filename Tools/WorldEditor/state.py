import pygame
import struct

from enum import Enum

from Config.settings import *
from SpriteMaker.utils import write_binary_file
from WorldEditor.loader import load_map_and_registry
from WorldEditor.models import SpriteEntry, PlacedSprite


class ActiveElement(Enum):
    sidebar = 1,
    editor = 2


class EditorState:
    def __init__(self):
        self.screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        pygame.display.set_caption("World Editor")
        self.clock = pygame.time.Clock()
        self.font = pygame.font.SysFont(None, 18)

        self.cam_x, self.cam_y = 0, 0
        self.zoom_levels = [1, 2, 3, 4]
        self.zoom_index = 0
        self.zoom = self.zoom_levels[self.zoom_index]

        self.sprite_library: list[SpriteEntry] = []
        self.active_sprite_library: list[SpriteEntry] = []
        self.selected_sprite_index: int = -1
        self.sprite_rects: list[tuple[pygame.Rect, int]] = []
        self.sprite_scroll_offset: int = 0

        self.loaded_sprite_data = SpriteEntry | None
        self.loaded_sprite_size = (0, 0)

        self.placed_sprites: list[PlacedSprite] = []

        self.editor_width = EDITOR_WIDTH
        self.sidebar_width = SIDEBAR_WIDTH
        self.screen_height = SCREEN_HEIGHT
        self.tile_size = TILE_SIZE
        self.chunk_w = CHUNK_PIXEL_WIDTH
        self.chunk_h = CHUNK_PIXEL_HEIGHT

        self.placing_sprite = False
        self.ghost_sprite_pos = None  # (grid_x, grid_y) or None

        # World bounding box (min_x, min_y, max_x, max_y)
        self.world_bounds = [float("inf"), float("inf"), float("-inf"), float("-inf")]

        self.active_element = ActiveElement.editor

    def world_to_screen(self, x, y):
        return int((x - self.cam_x) * self.zoom), int((y - self.cam_y) * self.zoom)

    def screen_to_world(self, mx, my):
        return mx / self.zoom + self.cam_x, my / self.zoom + self.cam_y

    def adjust_zoom(self, direction):
        self.zoom_index = max(0, min(self.zoom_index + direction, len(self.zoom_levels) - 1))
        self.zoom = self.zoom_levels[self.zoom_index]

    def place_sprite(self) -> None:
        gx, gy = self.ghost_sprite_pos
        sprite = PlacedSprite(gx, gy, gx // (CHUNK_PIXEL_WIDTH / TILE_SIZE), gy // (CHUNK_PIXEL_HEIGHT / TILE_SIZE),
                              self.loaded_sprite_data)
        self.placed_sprites.append(sprite)
        self.placing_sprite = False
        self.ghost_sprite_pos = None

        print(sprite)

        w = len(sprite.sprite_entry.sprite_data[0])
        h = len(sprite.sprite_entry.sprite_data)
        self.world_bounds[0] = min(self.world_bounds[0], gx)
        self.world_bounds[1] = min(self.world_bounds[1], gy)
        self.world_bounds[2] = max(self.world_bounds[2], gx + w)
        self.world_bounds[3] = max(self.world_bounds[3], gy + h)

    def _move_cam(self, event) -> None:
        if event.key == pygame.K_LEFT:
            self.cam_x -= 32
        elif event.key == pygame.K_RIGHT:
            self.cam_x += 32
        elif event.key == pygame.K_UP:
            self.cam_y -= 32
        elif event.key == pygame.K_DOWN:
            self.cam_y += 32

    def _scroll_sprites(self, event) -> None:
        if event.key == pygame.K_UP and self.sprite_scroll_offset > 1:
            self.sprite_scroll_offset -= 1
        elif event.key == pygame.K_DOWN and self.sprite_scroll_offset < max(0, len(self.active_sprite_library) - 1):
            self.sprite_scroll_offset += 1

    def handle_key(self, event):
        if self.active_element == ActiveElement.editor:
            self._move_cam(event)
        else:
            self._scroll_sprites(event)

        self.active_sprite_library = self.sprite_library[self.sprite_scroll_offset:]

        if event.key in (pygame.K_EQUALS, pygame.K_KP_PLUS):
            self.adjust_zoom(1)
        elif event.key in (pygame.K_MINUS, pygame.K_KP_MINUS):
            self.adjust_zoom(-1)

    def load(self):
        load_result = load_map_and_registry(TILE_SIZE)
        if load_result:
            _, _, placed_sprites, sprite_db = load_result

            self.placed_sprites.extend(placed_sprites)
        else:
            print(load_result, 'result')

    def save(self):
        """
        Serializes and saves the placed sprites to a `.cxmap` file using binary format.

        Format:
        - uint16: MAP_VERSION
        - uint32: chunk width in tiles
        - uint32: chunk height in tiles
        - For each chunk:
            - uint32: sprite count
            - For each sprite:
                - uint32: position_x (pixels)
                - uint32: position_y (pixels)
                - uint32: sprite_id
            - uint64: chunk end marker (0xFFFFFFFFFFFFFFFF)
        """
        if len(self.placed_sprites) < 1:
            return

        self.placed_sprites.sort(key=lambda sprite: (sprite.grid_y, sprite.grid_x))
        export_sprite_registry('sprites.cxreg', self.placed_sprites)
        buffer = bytearray()

        # Header
        buffer.extend(struct.pack('<H', MAP_VERSION))
        buffer.extend(struct.pack('<I', CHUNK_PIXEL_WIDTH // TILE_SIZE))
        buffer.extend(struct.pack('<I', CHUNK_PIXEL_HEIGHT // TILE_SIZE))

        current_chunk_x = self.placed_sprites[0].grid_x
        current_chunk_y = self.placed_sprites[0].grid_y
        current_chunk_data = []

        def flush_chunk_data(sprite_data):
            buffer.extend(struct.pack('<I', len(sprite_data)))  # sprite count
            for pos_x, pos_y, sprite_id in sprite_data:
                buffer.extend(struct.pack('<III', pos_x, pos_y, sprite_id))
            buffer.extend(struct.pack('<Q', 0xFFFFFFFFFFFFFFFF))  # chunk end marker

        for sprite in self.placed_sprites:
            sprite_chunk_x, sprite_chunk_y = sprite.grid_x, sprite.grid_y

            if (sprite_chunk_x != current_chunk_x) or (sprite_chunk_y != current_chunk_y):
                flush_chunk_data(current_chunk_data)
                current_chunk_data = []
                current_chunk_x, current_chunk_y = sprite_chunk_x, sprite_chunk_y

            current_chunk_data.append((
                sprite.position_x,
                sprite.position_y,
                sprite.sprite_entry.sprite_id
            ))

        # Flush final chunk
        flush_chunk_data(current_chunk_data)

        write_binary_file('test.cxmap', buffer)


def export_sprite_registry(file_path: str, placed_sprites: list[PlacedSprite]):
    """
    Writes a sprite ID → sprite path mapping file from placed sprites.
    Only unique (path, ID) pairs are recorded.
    """
    seen = set()
    lines = []

    for sprit in placed_sprites:
        entry = sprit.sprite_entry
        key = (entry.sprite_id, entry.sprite_path)
        if key not in seen:
            seen.add(key)
            lines.append(f"{entry.sprite_id} = {entry.sprite_path}")

    try:
        with open(file_path, 'w') as f:
            f.write("# Sprite ID registry\n")
            f.write("\n".join(lines))
        print(f"Sprite registry saved to: {file_path}")
    except Exception as e:
        print(f"Failed to write sprite registry: {e}")
