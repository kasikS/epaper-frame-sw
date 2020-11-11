#!/usr/bin/python
from PIL import Image
import sys

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


# PIL accesses images in Cartesian co-ordinates, so it is Image[columns, rows]
img = Image.new( 'RGB', (WIDTH,HEIGHT), "white") # create a new black image
pixels = img.load() # create the pixel map

with open(sys.argv[1], 'rb') as f:
    for y in range(HEIGHT):
        pixel_row = f.read(WIDTH // 2)

        for x in range(WIDTH // 2):
            pixels[2*x,     y] = PALETTE[pixel_row[x] >> 4]
            pixels[2*x + 1, y] = PALETTE[pixel_row[x] & 0x0f]

img.show()
