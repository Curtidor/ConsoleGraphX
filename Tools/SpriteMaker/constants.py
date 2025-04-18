GRID_SIZE_X = 6
GRID_SIZE_Y = 8
CANVAS_WIDTH, CANVAS_HEIGHT = 64, 64  # Grid dimensions
PALETTE_SIZE = 16
SPRITE_ID = 1
SCREEN_WIDTH = max(CANVAS_WIDTH * GRID_SIZE_X, PALETTE_SIZE * GRID_SIZE_X)
SCREEN_HEIGHT = (CANVAS_HEIGHT + 1) * GRID_SIZE_Y

EXPORT_FILE_PATH = "wall.cxsp"


DEFAULT_COLOR = "00 00"
FULL_BLOCK_CHAR = "25 88"
TRANSPARENT_CHAR = "20 0e"

TRANSPARENT_COLOR = (0, 0, 0)


# Custom palette colors
PALETTE_COLORS_x = [
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

PALETTE_COLORS = [
    (133, 149, 161),
    (109, 194, 202),
    (117, 113, 97),
    (78, 74, 78),
    (68, 36, 52),
    (210, 170, 153),
    (208, 70, 72),
    (218, 212, 94),
    (133, 76, 48),
    (210, 125, 44),
    (109, 170, 44),
    (222, 238, 214),
    (52, 101, 36),
    (89, 125, 206),
    (20, 12, 28),
    (48, 52, 109),
]


