#!/bin/sh
rm images/*.bin
find images -type f -exec ./convert_image.py \{\} \;
