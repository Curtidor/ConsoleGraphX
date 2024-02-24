from enum import Enum


class Colors(Enum):
    Black = 0
    DarkBlue = 1
    DarkGreen = 2
    DarkCyan = 3
    DarkRed = 4
    DarkMagenta = 5
    DarkYellow = 6
    Gray = 7
    DarkGray = 8
    Blue = 9
    Green = 10
    Cyan = 11
    Red = 12
    Magenta = 13
    Yellow = 14
    White = 15


ascii_art_to_convert = """
 ██████  ██████  ███    ██ ███████  ██████  ██      ███████      ██████  ██████   █████  ██████  ██   ██       ██   ██ 
██      ██    ██ ████   ██ ██      ██    ██ ██      ██          ██       ██   ██ ██   ██ ██   ██ ██   ██        ██ ██  
██      ██    ██ ██ ██  ██ ███████ ██    ██ ██      █████       ██   ███ ██████  ███████ ██████  ███████ █████   ███   
██      ██    ██ ██  ██ ██      ██ ██    ██ ██      ██          ██    ██ ██   ██ ██   ██ ██      ██   ██        ██ ██  
 ██████  ██████  ██   ████ ███████  ██████  ███████ ███████      ██████  ██   ██ ██   ██ ██      ██   ██       ██   ██ """


rows = [row for row in ascii_art_to_convert.split('\n') if row]

width = len(rows[0])
height = len(rows)

color = Colors.White
sprite_name = 't'

sprite_data = "{"
for index, char in enumerate(''.join(rows)):
    sprite_data += ',' if index > 0 else ''
    sprite_data += f"{{L'{char}', {color.value}}}" if char != " " else '{Screen::s_transparentPixel, 0}'

sprite_data += "}"

print(f"CHAR_INFO* {sprite_name} = new CHAR_INFO[{width*height}] {sprite_data};")
print(width)
print(height)
