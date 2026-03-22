# Boot Screen

The boot screen of the device is specified by the `./resources/boot_screen.tga` file. This file format and encoding is required by the Raspberry Pi bootloader,

Imagemagik

```
convert resources/boot_screen.png \
  -resize 800x480 \
  -background black \
  -gravity center \
  -extent 800x480 \
  -depth 8 \
  -colors 224 \
  -type truecolor \
  resources/boot_screen.tga
```