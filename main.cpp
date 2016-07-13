#include <iostream>
#include <SDL2/SDL.h>
#include "chip8.h"

using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 320;

int main(int argc, char** argv) {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window *window = SDL_CreateWindow(
			"chip8 emulator v0.1",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			0);

	SDL_Surface *screen = SDL_GetWindowSurface(window);

	Uint32 bgcolor = SDL_MapRGB(screen->format, 0x00, 0x00, 0x10);
	Uint32 fgcolor = SDL_MapRGB(screen->format, 0x00, 0xA0, 0xA0);

	bool running = true;

	Chip8 cpu;

	cpu.emulateCycle();

	while (running) {
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					running = false;
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
						case SDLK_ESCAPE:
							running = false;
							break;
					}
					break;

			}
		}

		// Clear the screen
		SDL_FillRect(screen, &screen->clip_rect, bgcolor);
		SDL_Rect rect;
		rect.x = 0;
		rect.y = 0;
		rect.w = 50;
		rect.h = 50;
		SDL_FillRect(screen, &rect, fgcolor);
		// Update screen
		SDL_UpdateWindowSurface(window);
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
