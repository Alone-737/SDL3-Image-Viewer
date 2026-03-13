# SDL3 Image Viewer 🖼️

A simple PPM image viewer written in C using SDL3. Pipe in a PPM file, get a window. That's it.

## Dependencies

You'll need:
- **GCC** (or any C compiler)
- **SDL3** — install it with:
  ```bash
  # Arch Linux
  sudo pacman -S sdl3
  ```

## Usage

Compile it:
```bash
gcc main.c -o main -lSDL3
```

Run it by piping a PPM file in:
```bash
cat your_image.ppm | ./main
```

Close the window to exit. That's literally all there is to it.

## Screenshots

![screenshot](screenshot.png)

## Notes

- Only supports **P6 (binary) PPM** files
- Window size is automatically set from the image dimensions
- No zoom, no pan, no frills — just pixels on a screen 🙂
