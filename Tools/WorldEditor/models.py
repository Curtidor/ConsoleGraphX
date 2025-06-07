from dataclasses import dataclass


@dataclass
class SpriteData:
    char: int
    color: int


@dataclass
class SpriteEntry:
    sprite_id: int
    sprite_data: list[list[SpriteData]]
    sprite_path: str


@dataclass
class PlacedSprite:
    position_x: int
    position_y: int
    grid_x: int
    grid_y: int
    sprite_entry: SpriteEntry
