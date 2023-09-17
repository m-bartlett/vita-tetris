#!/usr/bin/env python3
import sys
import pathlib
from PIL import Image


image_path = sys.argv[1]
image=Image.open(image_path)

palette = image.getpalette()
palette_size = len(palette)//3

enum_palette_serialized = [c for i in range(palette_size) for c in (i,i,i)]
image.putpalette(bytes(enum_palette_serialized))

image=image.convert('L')  # monochrome

try:
    save_path = sys.argv[2]
except IndexError:
    p = pathlib.Path(image_path)
    save_path = str(p.with_stem(f"modified_{p.stem}"))

print(f"wrote {save_path}")
image.save(save_path)