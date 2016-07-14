#include <iostream>
#include <SDL2/SDL.h>
#include "chip8.h"

using namespace std;

const int SCREEN_WIDTH = 64;
const int SCREEN_HEIGHT = 32;
const int FACTOR = 10;

int main(int argc, char** argv) {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window *window = SDL_CreateWindow(
			"chip8 emulator v0.1",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			SCREEN_WIDTH * FACTOR,
			SCREEN_HEIGHT * FACTOR,
			0);

	SDL_Surface *screen = SDL_GetWindowSurface(window);

	Uint32 bgcolor = SDL_MapRGB(screen->format, 0x00, 0x00, 0x10);
	Uint32 fgcolor = SDL_MapRGB(screen->format, 0x00, 0xA0, 0xA0);

	bool running = true;

	Chip8 cpu;

	if (argc > 1) {
		cpu.loadGame(argv[1]);
	}

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
					cpu.setKeystate(event.key.keysym.sym, 1);
					break;
				case SDL_KEYUP:
					cpu.setKeystate(event.key.keysym.sym, 0);
					break;

			}
		}

		cpu.emulateCycle();

		if (cpu.timeToDraw()) {
			// Clear the screen
			SDL_FillRect(screen, &screen->clip_rect, bgcolor);
			for (int y=0; y<SCREEN_HEIGHT; y++) {
				for (int x=0; x<SCREEN_WIDTH; x++) {
					if (cpu.gfx[x + (y * SCREEN_WIDTH)] != 0) {
						// Draw pixel
						SDL_Rect rect;
						rect.x = x * FACTOR;
						rect.y = y * FACTOR;
						rect.w = FACTOR;
						rect.h = FACTOR;
						SDL_FillRect(screen, &rect, fgcolor);
					}
				}
			}
			SDL_UpdateWindowSurface(window);
		}

		// Clear the screen
//		SDL_FillRect(screen, &screen->clip_rect, bgcolor);
//		SDL_Rect rect;
//		rect.x = 0;
//		rect.y = 0;
//		rect.w = 50;
//		rect.h = 50;
//		SDL_FillRect(screen, &rect, fgcolor);
//		// Update screen
//		SDL_UpdateWindowSurface(window);
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
