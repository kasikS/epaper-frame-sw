#!/usr/bin/python
from PIL import Image
import sys
import os
import math

WIDTH = 600
HEIGHT = 448
IMAGE_SIZE = math.ceil(WIDTH * HEIGHT / 2 / 512) * 512 # round to 512
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

if len(sys.argv) == 1:
    print('usage: show_image.py <file.bin> [image index]')
    exit()
if len(sys.argv) == 3:
    index = int(sys.argv[2])
else:
    index = 0


file_name = sys.argv[1]
if not os.path.isfile(file_name):
    print('ERROR: cannot open the input file')
    exit(1)


file_image_count = os.path.getsize(file_name) // IMAGE_SIZE
print('Number of images in the file: {0}'.format(file_image_count))

if index >= file_image_count:
    print('ERROR: requested image index is not available')
    exit(1)


img = Image.new('RGB', (WIDTH, HEIGHT))
pixels = img.load()

with open(file_name, 'rb') as f:
    # move the file pointer to the requested image
    f.seek(index * IMAGE_SIZE)

    for y in range(HEIGHT):
        pixel_row = f.read(WIDTH // 2)

        if not pixel_row:
            exit()

        for x in range(WIDTH // 2):
            pixels[2*x,     y] = PALETTE[pixel_row[x] >> 4]
            pixels[2*x + 1, y] = PALETTE[pixel_row[x] & 0x0f]

    img.show()
