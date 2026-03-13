#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<SDL3/SDL.h>

int main(){
    printf("Hello World\n");
    char *nousestring=calloc(1000,sizeof(char));
    fgets(nousestring,1000,stdin);
    while(1){
        fgets(nousestring,1000,stdin);
        if(nousestring[0] != '#') break;
    }
    int width=-1, height=-1;
    sscanf(nousestring,"%d %d",&width,&height);
    free(nousestring);

    char *buf=calloc(1000,sizeof(char));
    fgets(buf,1000,stdin);
    free(buf);
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window=SDL_CreateWindow("Image Viewer",width,height,0);
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    const SDL_PixelFormatDetails *fmt = SDL_GetPixelFormatDetails(surface->format);
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            Uint8 r=(Uint8) getchar();
            Uint8 g=(Uint8) getchar();
            Uint8 b=(Uint8) getchar();
            Uint32 color =SDL_MapRGB(fmt,NULL,r,g,b);
            SDL_Rect pixel={x,y,1,1};
            SDL_FillSurfaceRect(surface,&pixel,color);
            }
        }
    SDL_UpdateWindowSurface(window);
   
    SDL_Event event;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = 0;
            }
        }
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}