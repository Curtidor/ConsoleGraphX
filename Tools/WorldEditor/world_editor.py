import pygame
import sys

from WorldEditor.state import EditorState, ActiveElement
from WorldEditor.sidebar import draw_sidebar, handle_sidebar_click
from WorldEditor.grid import draw_grid, draw_sprites, get_hover_tile, draw_ghost_sprite
from Config.settings import BG_COLOR, HOVER_COLOR, TILE_SIZE


def main():
    pygame.init()
    state = EditorState()

    running = True
    while running:
        state.screen.fill(BG_COLOR)
        draw_grid(state)
        draw_sprites(state)
        draw_ghost_sprite(state)
        draw_sidebar(state)

        mouse_pos = pygame.mouse.get_pos()
        hover_tile = get_hover_tile(mouse_pos, state)

        if hover_tile:
            chunk_x, chunk_y, tile_x, tile_y = hover_tile
            global_x = chunk_x * (state.chunk_w // state.tile_size) + tile_x
            global_y = chunk_y * (state.chunk_h // state.tile_size) + tile_y
            world_x = global_x * state.tile_size
            world_y = global_y * state.tile_size
            sx, sy = state.world_to_screen(world_x, world_y)
            size = int(state.tile_size * state.zoom)
            pygame.draw.rect(state.screen, HOVER_COLOR, pygame.Rect(sx, sy, size, size), 2)

        if state.placing_sprite and hover_tile:
            grid_x = int(state.screen_to_world(mouse_pos[0], mouse_pos[1])[0]) // state.tile_size
            grid_y = int(state.screen_to_world(mouse_pos[0], mouse_pos[1])[1]) // state.tile_size
            state.ghost_sprite_pos = (grid_x, grid_y)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.MOUSEBUTTONDOWN:
                if event.pos[0] < state.editor_width:
                    if event.button == 1 and state.placing_sprite and state.ghost_sprite_pos:
                        state.place_sprite()
                    state.active_element = ActiveElement.editor
                else:
                    state.active_element = ActiveElement.sidebar
                    handle_sidebar_click(event.pos, state)

            elif event.type == pygame.KEYDOWN:
                state.handle_key(event)

                if event.key == pygame.K_s:
                    state.save()
                elif event.key == pygame.K_l:
                    state.load()

        pygame.display.flip()
        state.clock.tick(60)

    pygame.quit()
    sys.exit()


if __name__ == "__main__":
    main()
