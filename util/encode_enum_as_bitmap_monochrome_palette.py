#!/usr/bin/env python3
import sys
import pathlib
from PIL import Image

class Tetromino: 
    NULL = 0
    I    = 1
    O    = 2
    T    = 3
    J    = 4
    L    = 5
    S    = 6
    Z    = 7

enum_encoded_palette = { Tetromino.NULL: (0,0,0),
                         Tetromino.I: (Tetromino.I,Tetromino.I,Tetromino.I),
                         Tetromino.O: (Tetromino.O,Tetromino.O,Tetromino.O),
                         Tetromino.T: (Tetromino.T,Tetromino.T,Tetromino.T),
                         Tetromino.J: (Tetromino.J,Tetromino.J,Tetromino.J),
                         Tetromino.L: (Tetromino.L,Tetromino.L,Tetromino.L),
                         Tetromino.S: (Tetromino.S,Tetromino.S,Tetromino.S),
                         Tetromino.Z: (Tetromino.Z,Tetromino.Z,Tetromino.Z) }

palette_serialized = [v for c in enum_encoded_palette.values() for v in c] 
palette_serialized.extend([0]*(48-len(palette_serialized)))
data = bytes(palette_serialized)

image_path = sys.argv[1]
image=Image.open(image_path)
image.putpalette(data)
image=image.convert('L')  # monochrome

p = pathlib.Path(image_path)
save_path = str(p.with_stem(f"modified_{p.stem}"))
image.save(save_path)