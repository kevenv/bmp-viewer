#include <cstdio>
#include <string>

#include <SDL2/SDL.h>

#include "Bitmap.h"

void startSDL(Bitmap& bitmap);
void PutPixel24(SDL_Surface* surface, int x, int y, Uint32 color);

int main(int argc, char* argv[])
{
	if (argc != 2) return 1;
	std::string filePath = argv[1];

	Bitmap image(filePath);
	if (image.isLoaded()) {
		startSDL(image);
	}

	return 0;
}

void PutPixel24(SDL_Surface* surface, int x, int y, Uint32 color)
{
	Uint8* pixel = (Uint8*)surface->pixels;
	pixel += (y * surface->pitch) + (x * sizeof(Uint8) * 4);

	pixel[0] = (color >> 0) & 0xFF; //a
	pixel[1] = (color >> 8) & 0xFF; //r
	pixel[2] = (color >> 16) & 0xFF; //g
	pixel[3] = (color >> 24) & 0xFF; //b
}

void startSDL(Bitmap& bitmap)
{
	BITMAP_INFO_HEADER& infoHeader = bitmap.getInfoHeader();

	//init SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		return;
	}
	SDL_Window* window = SDL_CreateWindow("BMP Viewer", 100, 100, infoHeader.width, infoHeader.height, SDL_WINDOW_SHOWN);
	if (!window) {
		SDL_Quit();
		return;
	}
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		SDL_DestroyWindow(window);
		SDL_Quit();
		return;
	}

	//Create SW surface
	SDL_Surface* surface = SDL_CreateRGBSurface(0, infoHeader.width, infoHeader.height, 24, 
		0x0000FF00, 0x00FF0000, 0xFF000000, 0x00000000);
	if (!surface) {
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return;
	}

	//Load BMP
	uint8* pixels = bitmap.getPixels();
	SDL_LockSurface(surface);
	for (int y = 0; y < infoHeader.height; y++) {
		for (int x = 0; x < infoHeader.width; x++) {
			int i = x*3 + (infoHeader.height-y-1)*infoHeader.width*3;
			uint8 r = pixels[i+2];
			uint8 g = pixels[i+1];
			uint8 b = pixels[i+0];
			//printf("%X,%X,%X ", pixels[i + 2], pixels[i + 1], pixels[i + 0]);
			uint32 color = SDL_MapRGB(surface->format, r, g, b);
			PutPixel24(surface, x, y, color);
		}
	}
	SDL_UnlockSurface(surface);

	//Create HW surface
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!surface) {
		SDL_FreeSurface(surface);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return;
	}
	SDL_FreeSurface(surface);

	//Main loop
	bool running = true;
	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
				running = false;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		
		//Draw BMP
		SDL_Rect dst;
		dst.x = 0;
		dst.y = 0;
		dst.w = infoHeader.width;
		dst.h = infoHeader.height;
		SDL_RenderCopy(renderer, texture, NULL, &dst);

		SDL_RenderPresent(renderer);
		SDL_Delay(100);
	}
	
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}