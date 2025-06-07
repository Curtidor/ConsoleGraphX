# ConsoleGraphX File Format Reference

This document describes the custom file formats used in the ConsoleGraphX engine and tooling pipeline.

---

## `.cxmap` - Chunked World Map

### Description
Stores a world map composed of chunks, where each chunk contains sprite placements (positions + sprite IDs).

### Structure

| Field               | Size    | Type       | Description                              |
|--------------------|---------|------------|------------------------------------------|
| `MAP_VERSION`      | 2 bytes | `uint16`   | Version identifier for the map format    |
| `chunk_width`      | 4 bytes | `uint32`   | Width of a chunk in tiles                |
| `chunk_height`     | 4 bytes | `uint32`   | Height of a chunk in tiles               |
| **[per chunk]**    |         |            |                                          |
| `sprite_count`     | 4 bytes | `uint32`   | Number of sprites in this chunk          |
| **[per sprite]**   |         |            |                                          |
| `x`                | 4 bytes | `int32`    | X position in world space                |
| `y`                | 4 bytes | `int32`    | Y position in world space                |
| `sprite_id`        | 4 bytes | `int32`    | ID used to look up the sprite from `.cxreg` |
| `terminator`       | 8 bytes | `uint64`   | Optional chunk terminator (can be ignored or used as padding) |

---

## `.cxreg` - Sprite Registry

### Description
Maps sprite IDs used in `.cxmap` files to sprite asset paths on disk.

### Format

Plaintext INI-style format:

# Sprite registry
0 = path/to/sprite_0.cxsp

1 = path/to/sprite_1.cxsp



### Rules

- Lines with `=` are parsed as `ID = path`.
- Keys must be integers.
- Whitespace is trimmed.
- Comments (`#`) and empty lines are ignored.

---

## `.cxsp` - Sprite File

### Description
Stores a single sprite tile sheet in a format similar to Windows `CHAR_INFO`, with a header and per-tile data.

### Structure

| Field             | Size    | Type     | Description                               |
|------------------|---------|----------|-------------------------------------------|
| `version`        | 4 bytes | `uint32` | Version of the sprite file format         |
| `width`          | 4 bytes | `uint32` | Width in tiles                            |
| `height`         | 4 bytes | `uint32` | Height in tiles                           |
| `sprite_id`      | 4 bytes | `uint32` | Unique identifier for this sprite         |
| **[per tile]**   |         |          |                                           |
| `char_code`      | 2 bytes | `uint16` | Unicode code point (used for visuals)     |
| `color_attr`     | 2 bytes | `uint16` | 16-bit color index or style flags         |

### Notes

- `char_code` is usually set to:
  - `0x2588` (█) for visible pixels
  - `0x0020` (space) for transparent tiles
- `color_attr` matches a palette index or is `0x0000` for transparent

---

## Summary

| File Type | Purpose                        | Read By        |
|-----------|--------------------------------|----------------|
| `.cxmap`  | World layout (chunks/sprites)  | Game Engine    |
| `.cxreg`  | Sprite ID to file path mapping | Game Engine    |
| `.cxsp`   | Individual sprite definitions  | Engine & Editor|

