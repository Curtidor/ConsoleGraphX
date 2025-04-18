import pygame
from .constants import *


class Canvas:
    def __init__(self):
        self.canvas = [[None for _ in range(CANVAS_WIDTH)] for _ in range(CANVAS_HEIGHT)]
        self.selected_color = PALETTE_COLORS[0]
        self.selected_color_index = 0
        self.mouse_dragging = False

    def draw_grid(self, screen):
        # Draw the canvas
        for y in range(CANVAS_HEIGHT):
            for x in range(CANVAS_WIDTH):
                color = self.canvas[y][x] if self.canvas[y][x] is not None else TRANSPARENT_COLOR
                pygame.draw.rect(screen, color, pygame.Rect(x * GRID_SIZE_X, y * GRID_SIZE_Y, GRID_SIZE_X, GRID_SIZE_Y))

        # Draw the palette at the bottom left
        palette_start_x = 0  # Start drawing from the left edge of the screen
        palette_start_y = SCREEN_HEIGHT - GRID_SIZE_Y  # Bottom of the screen

        for i, color in enumerate(PALETTE_COLORS):
            pygame.draw.rect(
                screen,
                color,
                pygame.Rect(
                    palette_start_x + i * GRID_SIZE_X,
                    palette_start_y,
                    GRID_SIZE_X,
                    GRID_SIZE_Y
                )
            )

    def handle_drawing(self, mouse_x, mouse_y):
        if mouse_y < CANVAS_HEIGHT * GRID_SIZE_Y:  # Ensure we are drawing on the canvas
            grid_x = mouse_x // GRID_SIZE_X
            grid_y = mouse_y // GRID_SIZE_Y
            if 0 <= grid_x < CANVAS_WIDTH and 0 <= grid_y < CANVAS_HEIGHT:
                self.canvas[grid_y][grid_x] = self.selected_color

    def fill_canvas(self):
        for y in range(CANVAS_HEIGHT):
            for x in range(CANVAS_WIDTH):
                self.canvas[y][x] = self.selected_color

    def select_color(self, palette_index):
        if 0 <= palette_index < PALETTE_SIZE:
            self.selected_color = PALETTE_COLORS[palette_index]
            self.selected_color_index = palette_index
        elif palette_index == -1:
            self.selected_color = TRANSPARENT_COLOR
