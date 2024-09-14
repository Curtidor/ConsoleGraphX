import pygame
import sys

# Pygame setup
pygame.init()

# Constants
GRID_SIZE = 4  # Each cell size (for drawing)
CANVAS_WIDTH, CANVAS_HEIGHT = 300, 10  # Grid dimensions
PALETTE_SIZE = 16
SCREEN_WIDTH = CANVAS_WIDTH * GRID_SIZE
SCREEN_HEIGHT = max(((CANVAS_HEIGHT * GRID_SIZE) + PALETTE_SIZE * GRID_SIZE), 500)
TRANSPARENT_COLOR = (0, 0, 0)  # Background (transparent)

# Custom palette colors
palette_colors = [
    (242, 192, 162),  # RGB(242, 192, 162)
    (233, 132, 114),  # RGB(233, 132, 114)
    (216, 35, 35),    # RGB(216, 35, 35)
    (152, 24, 60),    # RGB(152, 24, 60)
    (31, 203, 35),    # RGB(31, 203, 35)
    (18, 109, 48),    # RGB(18, 109, 48)
    (38, 221, 221),   # RGB(38, 221, 221)
    (24, 103, 160),   # RGB(24, 103, 160)
    (147, 66, 38),    # RGB(147, 66, 38)
    (108, 37, 30),    # RGB(108, 37, 30)
    (247, 226, 108),  # RGB(247, 226, 108)
    (237, 179, 41),   # RGB(237, 179, 41)
    (231, 109, 20),   # RGB(231, 109, 20)
    (242, 242, 249),  # RGB(242, 242, 249)
    (106, 95, 160),   # RGB(106, 95, 160)
    (22, 20, 35)      # RGB(22, 20, 35)
]

# Initialize screen
screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
pygame.display.set_caption("Sprite Drawer")

# Sprite canvas array
canvas = [[None for _ in range(CANVAS_WIDTH)] for _ in range(CANVAS_HEIGHT)]

# Default selected color
selected_color = palette_colors[0]
selected_color_index = 0
mouse_dragging = False  # Variable to track if the mouse is dragging


# Function to draw the grid and palette
def draw_grid():
    # Draw the canvas
    for y in range(CANVAS_HEIGHT):
        for x in range(CANVAS_WIDTH):
            color = canvas[y][x] if canvas[y][x] is not None else TRANSPARENT_COLOR
            pygame.draw.rect(screen, color, pygame.Rect(x * GRID_SIZE, y * GRID_SIZE, GRID_SIZE, GRID_SIZE))

    # Draw the palette
    for i, color in enumerate(palette_colors):
        pygame.draw.rect(screen, color, pygame.Rect(i * GRID_SIZE, CANVAS_HEIGHT * GRID_SIZE, GRID_SIZE, GRID_SIZE))


# Function to export the sprite in the desired format
def export_sprite():
    output = []
    for y in range(CANVAS_HEIGHT):
        row_output = []
        for x in range(CANVAS_WIDTH):
            color = canvas[y][x]
            if color is None:
                row_output.append("{Screen::s_transparentPixel, 0}")
            else:
                color_index = palette_colors.index(color)
                row_output.append(f"{{L'█', {color_index}}}")
        output.append(", ".join(row_output))

    formatted_output = f"CHAR_INFO* t = new CHAR_INFO[{CANVAS_WIDTH * CANVAS_HEIGHT}] {{"
    formatted_output += ", ".join(output)
    formatted_output += "};"

    # Output to the console
    print(formatted_output)


# Function to handle drawing on the canvas
def handle_drawing(mouse_x, mouse_y):
    if mouse_y < CANVAS_HEIGHT * GRID_SIZE:  # Ensure we are drawing on the canvas
        grid_x = mouse_x // GRID_SIZE
        grid_y = mouse_y // GRID_SIZE
        if 0 <= grid_x < CANVAS_WIDTH and 0 <= grid_y < CANVAS_HEIGHT:
            canvas[grid_y][grid_x] = selected_color


# Function to fill the entire canvas with the selected color
def fill_canvas():
    for y in range(CANVAS_HEIGHT):
        for x in range(CANVAS_WIDTH):
            canvas[y][x] = selected_color


# Main loop
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
            pygame.quit()
            sys.exit()

        # Handle mouse clicks for drawing and palette selection
        if event.type == pygame.MOUSEBUTTONDOWN:
            mouse_x, mouse_y = event.pos
            if mouse_y >= CANVAS_HEIGHT * GRID_SIZE:
                # Select color from the palette
                palette_index = mouse_x // GRID_SIZE
                if 0 <= palette_index < PALETTE_SIZE:
                    selected_color = palette_colors[palette_index]
                    selected_color_index = palette_index
            else:
                # Start drawing
                mouse_dragging = True
                handle_drawing(mouse_x, mouse_y)

        if event.type == pygame.MOUSEBUTTONUP:
            mouse_dragging = False  # Stop drawing when the mouse is released

        if event.type == pygame.MOUSEMOTION:
            if mouse_dragging:  # Continue drawing as the mouse moves
                mouse_x, mouse_y = event.pos
                handle_drawing(mouse_x, mouse_y)

        # Handle keyboard input for exporting and filling
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_e:  # Press 'E' to export
                export_sprite()
            elif event.key == pygame.K_f:  # Press 'F' to fill the canvas
                fill_canvas()

    # Clear screen
    screen.fill(TRANSPARENT_COLOR)

    # Draw grid and palette
    draw_grid()

    # Update the display
    pygame.display.flip()
