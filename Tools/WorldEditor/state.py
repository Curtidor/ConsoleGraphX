import sys

import pygame

from enum import Enum
from dataclasses import dataclass

from Config.settings import *
from SpriteMaker.sprite_utils import SpriteData
from SpriteMaker.utils import (write_hex_file, int16_to_hex_string, int32_to_hex_string, int64_to_hex_string,
                               reverse_bytes_for_little_endian)


@dataclass
class SpriteEntry:
    sprite_id: int
    sprite_data: list[list[SpriteData]]


@dataclass
class PlacedSprite:
    position_x: int
    position_y: int
    grid_x: int
    grid_y: int
    sprite_entry: SpriteEntry


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
        sprite = PlacedSprite(gx, gy, gx//(CHUNK_PIXEL_WIDTH/TILE_SIZE), gy//(CHUNK_PIXEL_HEIGHT/TILE_SIZE),
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

    def save(self):
        if len(self.placed_sprites) < 1:
            return

        self.placed_sprites.sort(key=lambda sprite: (sprite.grid_y, sprite.grid_x))

        hex_data = [
            int16_to_hex_string(MAP_VERSION),
            int32_to_hex_string(CHUNK_PIXEL_WIDTH // TILE_SIZE),
            int32_to_hex_string(CHUNK_PIXEL_HEIGHT // TILE_SIZE)
        ]

        current_chunk_x = self.placed_sprites[0].grid_x
        current_chunk_y = self.placed_sprites[0].grid_y
        current_chunk_sprite_count = 0
        current_chunk_data = []

        def flush_chunk_data(grid_x, grid_y, sprite_count, sprite_data):
            hex_data.append(int32_to_hex_string(sprite_count))  # sprite count
            hex_data.extend(sprite_data)
            hex_data.append(int64_to_hex_string(0xFFFFFFFFFFFFFFFF))  # chunk terminator

        for sprite in self.placed_sprites:
            sprite_chunk_x, sprite_chunk_y = sprite.grid_x, sprite.grid_y

            if (sprite_chunk_x != current_chunk_x) or (sprite_chunk_y != current_chunk_y):
                flush_chunk_data(current_chunk_x, current_chunk_y, current_chunk_sprite_count, current_chunk_data)
                current_chunk_x, current_chunk_y = sprite_chunk_x, sprite_chunk_y
                current_chunk_data = []
                current_chunk_sprite_count = 0

            current_chunk_data.append(int32_to_hex_string(sprite.position_x))
            current_chunk_data.append(int32_to_hex_string(sprite.position_y))
            current_chunk_data.append(int32_to_hex_string(sprite.sprite_entry.sprite_id))
            current_chunk_sprite_count += 1

        # Flush last chunk
        flush_chunk_data(current_chunk_x, current_chunk_y, current_chunk_sprite_count, current_chunk_data)

        formatted_bytes = reverse_bytes_for_little_endian(hex_data) if sys.byteorder == "little" else hex_data
        byte_data = "".join(formatted_bytes).replace(" ", "")

        write_hex_file('test.cxmap', byte_data)



