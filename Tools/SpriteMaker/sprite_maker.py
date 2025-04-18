import pygame
import sys

from SpriteMaker.constants import (SCREEN_WIDTH, SCREEN_HEIGHT, CANVAS_WIDTH, CANVAS_HEIGHT, GRID_SIZE_X, GRID_SIZE_Y,
                                   EXPORT_FILE_PATH, SPRITE_ID)
from SpriteMaker.canvas import Canvas
from SpriteMaker.sprite_utils import export_sprite

from constants import TRANSPARENT_COLOR


def main():
    print("CONTROLS:")
    print('-'*40)
    print("F: to fill the screen with the currently selected color\nE: to export the sprite"
          "\nR: to go into erase mode\n\nEnjoy!\n")
    pygame.init()

    screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
    pygame.display.set_caption("Sprite Drawer")

    canvas = Canvas()

    # Main loop
    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
                pygame.quit()
                sys.exit()

            if event.type == pygame.MOUSEBUTTONDOWN:
                mouse_x, mouse_y = event.pos
                if mouse_y >= CANVAS_HEIGHT * GRID_SIZE_Y:
                    palette_index = mouse_x // GRID_SIZE_X
                    canvas.select_color(palette_index)
                else:
                    canvas.mouse_dragging = True
                    canvas.handle_drawing(mouse_x, mouse_y)

            if event.type == pygame.MOUSEBUTTONUP:
                canvas.mouse_dragging = False

            if event.type == pygame.MOUSEMOTION:
                if canvas.mouse_dragging:
                    mouse_x, mouse_y = event.pos
                    canvas.handle_drawing(mouse_x, mouse_y)

            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_e:
                    export_sprite(
                        sprite_width=CANVAS_WIDTH, sprite_height=CANVAS_HEIGHT,
                        colors=canvas.canvas, export_path=EXPORT_FILE_PATH, sprite_id=SPRITE_ID
                                  )

                elif event.key == pygame.K_f:
                    canvas.fill_canvas()
                elif event.key == pygame.K_r:
                    canvas.select_color(-1)

        screen.fill(TRANSPARENT_COLOR)
        canvas.draw_grid(screen)
        pygame.display.flip()


if __name__ == "__main__":
    main()

