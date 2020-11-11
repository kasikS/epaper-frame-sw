#!/usr/bin/python
from PIL import Image
import sys
import os

WIDTH = 600
HEIGHT = 448
PALETTE = (
    (0, 0, 0),
    (255, 255, 255),
    (67, 138, 28),
    (100, 64, 255),
    (191, 0, 0),
    (255, 243, 56),
    (232, 126, 0),
    (194, 164, 24)
)

def to_palette(color):
    min_diff = sys.maxsize
    color_index = None

    for idx, p in enumerate(PALETTE):
        diff_r = (color[0] - p[0])
        diff_g = (color[1] - p[1])
        diff_b = (color[2] - p[2])
        diff = diff_r**2 + diff_g**2 + diff_b**2

        if diff < min_diff:
            min_diff = diff
            color_index = idx

    return color_index



raw_data = []

with Image.open(sys.argv[1]) as input_img:
    input_pix = input_img.load()

    for y in range(HEIGHT):
        for x in range(WIDTH // 2):
            p1 = to_palette(input_pix[2*x,     y])
            p2 = to_palette(input_pix[2*x + 1, y])
            raw_data.append(p1 << 4 | p2)

with open(os.path.splitext(sys.argv[1])[0] + '.bin', 'wb') as output_file:
    output_file.write(bytes(raw_data))
