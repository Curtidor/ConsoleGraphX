import pygame
from Config.settings import GRID_COLOR
from SpriteMaker.constants import PALETTE_COLORS
from state import EditorState, SpriteEntry


def draw_grid(state: EditorState):
    step = int(state.tile_size * state.zoom)
    chunk_width_px = state.chunk_w * state.zoom
    chunk_height_px = state.chunk_h * state.zoom
    x_start = -int((state.cam_x % state.tile_size) * state.zoom)
    y_start = -int((state.cam_y % state.tile_size) * state.zoom)

    for x in range(x_start, state.editor_width, step):
        wx = (x + state.cam_x * state.zoom) / state.zoom
        color = (100, 100, 100) if int(wx) % state.chunk_w == 0 else GRID_COLOR
        pygame.draw.line(state.screen, color, (x, 0), (x, state.screen_height))

    for y in range(y_start, state.screen_height, step):
        wy = (y + state.cam_y * state.zoom) / state.zoom
        color = (100, 100, 100) if int(wy) % state.chunk_h == 0 else GRID_COLOR
        pygame.draw.line(state.screen, color, (0, y), (state.editor_width, y))


def draw_sprites(state: EditorState):
    for placed in state.placed_sprites:
        for y, sprite_data_collection in enumerate(placed.sprite_entry.sprite_data):
            for x, sprite_data in enumerate(sprite_data_collection):
                if sprite_data.char == -1:
                    continue  # skip transparent tiles

                color = PALETTE_COLORS[sprite_data.color]
                world_x = (placed.position_x + x) * state.tile_size
                world_y = (placed.position_y + y) * state.tile_size
                sx, sy = state.world_to_screen(world_x, world_y)
                size = int(state.tile_size * state.zoom)
                pygame.draw.rect(state.screen, color, pygame.Rect(sx, sy, size, size))


def draw_ghost_sprite(state: EditorState):
    if not state.placing_sprite or not state.ghost_sprite_pos or not state.loaded_sprite_data:
        return
    gx, gy = state.ghost_sprite_pos
    for y, row in enumerate(state.loaded_sprite_data.sprite_data):
        for x, sprite_data in enumerate(row):
            color = PALETTE_COLORS[sprite_data.color]
            color = tuple(min(255, c + 50) for c in color)  # brighten for ghost
            world_x = (gx + x) * state.tile_size
            world_y = (gy + y) * state.tile_size
            sx, sy = state.world_to_screen(world_x, world_y)
            size = int(state.tile_size * state.zoom)
            pygame.draw.rect(state.screen, color, pygame.Rect(sx, sy, size, size), 1)  # outline only


def get_hover_tile(mouse_pos, state: EditorState):
    mx, my = mouse_pos
    if mx >= state.editor_width:
        return None
    wx, wy = state.screen_to_world(mx, my)
    tile_x = int(wx) // state.tile_size
    tile_y = int(wy) // state.tile_size
    chunk_x = tile_x * state.tile_size // state.chunk_w
    chunk_y = tile_y * state.tile_size // state.chunk_h
    tile_x %= (state.chunk_w // state.tile_size)
    tile_y %= (state.chunk_h // state.tile_size)
    return chunk_x, chunk_y, tile_x, tile_y
