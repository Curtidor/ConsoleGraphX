import pygame
from SpriteMaker.sprite_utils import open_sprite_file
from SpriteMaker.constants import PALETTE_COLORS
from state import EditorState, SpriteEntry


def draw_sidebar(state: EditorState):
    sidebar_x = state.editor_width
    pygame.draw.rect(state.screen, (20, 20, 20), (sidebar_x, 0, state.sidebar_width, state.screen_height))

    # Load Sprite Button
    load_button_width = 120
    load_button_y = 20
    load_button_x = state.editor_width + (state.sidebar_width - load_button_width) // 2
    load_button = pygame.Rect(load_button_x, load_button_y, load_button_width, 30)
    pygame.draw.rect(state.screen, (70, 70, 70), load_button)
    pygame.draw.rect(state.screen, (200, 200, 200), load_button, 1)
    label = state.font.render("Load Sprite", True, (255, 255, 255))
    state.screen.blit(label, (load_button.x + 10, load_button.y + 5))

    # Sprite Previews
    sprite_start_y = load_button_y + 50
    sprite_margin = 10
    sprite_cell_size = 6

    state.sprite_rects.clear()

    for idx, sprite_data_collection in enumerate(state.active_sprite_library):
        w = len(sprite_data_collection.sprite_data[0])
        h = len(sprite_data_collection.sprite_data)
        preview_area_width = state.sidebar_width - 40
        preview_area_height = 80

        cell_w = preview_area_width / w
        cell_h = preview_area_height / h
        cell_size = int(min(cell_w, cell_h, sprite_cell_size))

        sprite_y_offset = sprite_start_y + idx * (h * cell_size + sprite_margin)
        start_x = sidebar_x + (state.sidebar_width - w * cell_size) // 2

        sprite_rect = pygame.Rect(start_x, sprite_y_offset, w * cell_size, h * cell_size)
        state.sprite_rects.append((sprite_rect, idx))

        border_color = (255, 255, 255) if state.selected_sprite_index == idx else (100, 100, 100)
        pygame.draw.rect(state.screen, border_color, sprite_rect.inflate(4, 4), 1)

        for y, row in enumerate(sprite_data_collection.sprite_data):
            for x, sprite_data in enumerate(row):
                color = PALETTE_COLORS[sprite_data.color]
                rect = pygame.Rect(start_x + x * cell_size, sprite_y_offset + y * cell_size, cell_size, cell_size)
                pygame.draw.rect(state.screen, color, rect)


def handle_sidebar_click(pos, state):
    if pos[0] < state.editor_width:
        return

    mx, my = pos
    load_button_width = 120
    load_button_y = 20
    load_button_x = state.editor_width + (state.sidebar_width - load_button_width) // 2
    if (load_button_x <= mx <= load_button_x + load_button_width and
            load_button_y <= my <= load_button_y + 30):
        sprite_id, sprite_data, sprite_path = open_sprite_file()
        if sprite_data:
            state.sprite_library.append(SpriteEntry(sprite_id, sprite_data, sprite_path))
        return

    # Check for sprite selection
    for rect, idx in state.sprite_rects:
        if rect.collidepoint(mx, my):
            state.selected_sprite_index = idx
            state.loaded_sprite_data = state.active_sprite_library[idx]
            state.placing_sprite = True
            break
