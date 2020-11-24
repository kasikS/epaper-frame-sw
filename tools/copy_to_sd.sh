#!/bin/sh
cat images/*.bin sentinel.bin > sd_image.raw
sudo dd if=sd_image.raw of=/dev/mmcblk0 bs=512
