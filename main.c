#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL3/SDL.h>


FILE *fp;
int is_pipe;
int is_p3;
int width, height;

FILE* stdin_to_ffmpeg(unsigned char first2[2]) {
    char tmppath[] = "/tmp/imgviewer_XXXXXX";
    int fd = mkstemp(tmppath);
    FILE *tmp = fdopen(fd, "wb");

    fwrite(first2, 1, 2, tmp);

    unsigned char chunk[4096];
    size_t n;
    while ((n = fread(chunk, 1, sizeof(chunk), stdin)) > 0)
        fwrite(chunk, 1, n, tmp);

    fclose(tmp);

   
    char cmd[1000];
    snprintf(cmd, sizeof(cmd),
        "ffmpeg -i \"%s\" -f image2pipe -vcodec ppm pipe:1 2>/dev/null; rm \"%s\"",
        tmppath, tmppath);

    return popen(cmd, "r");
}

void open_image(const char *path) {

    FILE *f;
    int from_stdin = 0;

    if (path == NULL || strcmp(path, "-") == 0) {
        f = stdin;
        from_stdin = 1;
    } else {
        f = fopen(path, "rb");
        if (!f) {
            printf("Cannot open the file\n");
            exit(1);
        }

    }

    unsigned char magic[2] = {0};
    fread(magic, 1, 2, f);

    if (magic[0] == 'P' && magic[1] == '6') {
        fp = f;
        is_pipe = from_stdin;
        is_p3 = 0;

    } else if (magic[0] == 'P' && magic[1] == '3') {
        fp = f;
        is_pipe = from_stdin;
        is_p3 = 1;

    } else {
        if (from_stdin) {
            fp = stdin_to_ffmpeg(magic);
            is_pipe = 1;
            is_p3 = 0;
        } else {
            fclose(f);
            char cmd[1024];
            snprintf(cmd, sizeof(cmd),
                "ffmpeg -i \"%s\" -f image2pipe -vcodec ppm pipe:1 2>/dev/null",
                path);
            fp = popen(cmd, "r");
            is_pipe = 1;
            is_p3 = 0;
        }
    }
}

void read_ppm_header() {
    char buf[1000];
    fgets(buf, sizeof(buf), fp);

    do {
        fgets(buf, sizeof(buf), fp);
    } while (buf[0] == '#');
    
    sscanf(buf, "%d %d", &width, &height);
    fgets(buf, sizeof(buf), fp); 
}

void read_pixel_p3(FILE *fp, Uint8 *r, Uint8 *g, Uint8 *b) {
    int red, green, blue;
    fscanf(fp, "%d %d %d", &red, &green, &blue);
    *r = (Uint8)red;
    *g = (Uint8)green;
    *b = (Uint8)blue;
}

void read_pixel_p6(FILE *fp, Uint8 *r, Uint8 *g, Uint8 *b) {
    *r = (Uint8)fgetc(fp);
    *g = (Uint8)fgetc(fp);
    *b = (Uint8)fgetc(fp);
}

\
int main(int argc, char *argv[]) {
    const char *path = (argc >= 2) ? argv[1] : NULL;

    open_image(path);
    read_ppm_header();

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Image Viewer", width, height, 0);
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    const SDL_PixelFormatDetails *fmt = SDL_GetPixelFormatDetails(surface->format);

    SDL_LockSurface(surface);
    Uint32 *pixels = (Uint32*)surface->pixels;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint8 r, g, b;

            if (is_p3){
                read_pixel_p3(fp,&r, &g, &b);
            }else{
                read_pixel_p6(fp,&r, &g, &b);
            }

            pixels[y * (surface->pitch / 4) + x] = SDL_MapRGB(fmt, NULL, r, g, b);
        }
    }

    SDL_UnlockSurface(surface);

    is_pipe ? pclose(fp) : fclose(fp);

    SDL_UpdateWindowSurface(window);

    SDL_Event event;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event))
            if (event.type == SDL_EVENT_QUIT) running = 0;
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}