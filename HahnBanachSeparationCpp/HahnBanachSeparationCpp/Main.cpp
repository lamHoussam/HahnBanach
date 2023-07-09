#define SDL_MAIN_HANDLED

#include <iostream>
#include <SDL.h>

#define	APP_WIDTH  480
#define APP_HEIGHT 640


int main() {
	SDL_SetMainReady();

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("SDL2 Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, APP_WIDTH, APP_HEIGHT, SDL_WINDOW_SHOWN);


	bool isRunning = true;
	while (isRunning)
	{
		SDL_Event event;
		SDL_PollEvent(&event);


		switch (event.type) {
		case SDL_QUIT:
			isRunning = false;
			break;
		default:
			break;
		}
		// Render all objects here

	}




	std::cout << "Hello world" << std::endl;

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}