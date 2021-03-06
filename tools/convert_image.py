#!/usr/bin/python
from PIL import Image
import hitherdither
import sys
import os

WIDTH = 600
HEIGHT = 448
PALETTE = (
    (25, 25, 25),       # black
    (255, 255, 255),    # white
    (74, 129, 74),      # green
    (69, 68, 108),      # blue
    (153, 50, 69),      # red
    (235, 201, 104),     # yellow
    (225, 172, 118),     # orange
)

raw_data = []

# convert RGB palette to hitherdither compatible format
hitherdither_array = []
for p in PALETTE:
    hitherdither_array.append(p[0] << 16 | p[1] << 8 | p[2])
hitherdither_palette = hitherdither.palette.Palette(hitherdither_array)

with Image.open(sys.argv[1]) as input_img:
    # scale the image so at least one dimension matches the display resolution
    if input_img.size != (WIDTH, HEIGHT):
        scale = 1

        if input_img.size[0] > input_img.size[1]:
            # landscape
            scale = HEIGHT / input_img.size[1]
        else:
            # portrait
            scale = WIDTH / input_img.size[0]

        new_size = (int(input_img.size[0] * scale), int(input_img.size[1] * scale))
        print('Rescaling image to {0}'.format(new_size))
        assert(new_size[0] == WIDTH or new_size[1] == HEIGHT)
        input_img = input_img.resize(new_size)

    # crop the image so that it fits the display
    if input_img.size != (WIDTH, HEIGHT):
        width_crop = int((input_img.size[0] - WIDTH) / 2)
        height_crop = int((input_img.size[1] - HEIGHT) / 2)
        crop_rect = (width_crop, height_crop, WIDTH + width_crop, HEIGHT + height_crop)
        print('Cropping image {0}'.format(crop_rect))
        input_img = input_img.crop(crop_rect)

    assert(input_img.size[0] == WIDTH and input_img.size[1] == HEIGHT)

    #dithered_img = hitherdither.ordered.yliluoma.yliluomas_1_ordered_dithering(input_img, hitherdither_palette, order=8)
    #dithered_img = hitherdither.ordered.bayer.bayer_dithering(input_img, hitherdither_palette, [256/4, 256/4, 256/4], order=16)
    dithered_img = hitherdither.diffusion.error_diffusion_dithering(input_img, hitherdither_palette, method="floyd-steinberg", order=2)
    input_pix = dithered_img.load()

    for y in range(HEIGHT):
        for x in range(WIDTH // 2):
            p1 = input_pix[2*x,     y]
            p2 = input_pix[2*x + 1, y]
            raw_data.append(p1 << 4 | p2)

    # padding to make it a multiplicity of 512 bytes
    raw_data.extend([0xff] * 256)

with open(os.path.splitext(sys.argv[1])[0] + '.bin', 'wb') as output_file:
    output_file.write(bytes(raw_data))
