#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <string>

#include "res_path.h"
#include "cleanup.h"

//Screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int TILE_SIZE = 40;

/**
 * Log an SDL error with an error message to the output stream.
 *
 * @param os  The output stream to write the message to.
 * @param msg The error message to write, with format "{msg} error: {SDL_GetError()}".
 */
void logSDLError(std::ostream& os, const std::string& msg) {
	os << msg << " error: " << SDL_GetError() << std::endl;
}

/**
 * Loads an image into a texture on the rendering device.
 *
 * @param  file The image file to load.
 * @param  ren  The renderer to load the texture onto.
 * @return      The loaded texture, or nullptr if something went wrong.
 */
SDL_Texture* loadTexture(const std::string& file, SDL_Renderer* ren) {
	SDL_Texture* texture = IMG_LoadTexture(ren, file.c_str());

	if (texture == nullptr) {
		logSDLError(std::cout, "LoadTexture");
	}

	return texture;
}

/**
 * Draw an SDL_Texture to an SDL_Renderer at (x,y), with
 * specified width and height.
 *
 * @param tex The source texture we want to draw.
 * @param ren [The renderer we want to draw to.
 * @param x   The x coordinate to draw to.
 * @param y   The y coordinate to draw to.
 * @param w   The width of the texture to draw.
 * @param h   The height of the texture to draw.
 */
void renderTexture(SDL_Texture* tex, SDL_Renderer* ren, int x, int y, int w, int h) {
	//Setup destination rectangle at given (x,y)
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	dst.w = w;
	dst.h = h;
	SDL_RenderCopy(ren, tex, NULL, &dst);
}

/**
 * Draw an SDL_Texture to an SDL_Renderer at (x,y), preserving
 * the texture's width and height.
 *
 * @param tex The source texture we want to draw.
 * @param ren [The renderer we want to draw to.
 * @param x   The x coordinate to draw to.
 * @param y   The y coordinate to draw to.
 */
void renderTexture(SDL_Texture* tex, SDL_Renderer* ren, int x, int y) {
	int w, h;
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	renderTexture(tex, ren, x, y, w, h);
}

int main (int argc, char** argv) {
	/* SDL initialization */
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		logSDLError(std::cout, "SDL_Init");
		return 1;
	}
	/* Window initialization */
	SDL_Window* win = SDL_CreateWindow("Lesson 3", 100, 100,
		SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (win == nullptr) {
		logSDLError(std::cout, "CreateWindow");
		SDL_Quit();
		return 1;
	}
	/* Renderer initialization */
	SDL_Renderer *ren = SDL_CreateRenderer(win, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr) {
		logSDLError(std::cout, "CreateRenderer");
		cleanup(win);
		SDL_Quit();
		return 1;
	}

	/* Image initialization */
	const std::string resPath = getResourcePath("Lesson3");
	SDL_Texture* background = loadTexture(resPath + "background.png", ren);
	SDL_Texture* image = loadTexture(resPath + "image.png", ren);
	if (background == nullptr || image == nullptr) {
		cleanup(background, image, ren, win);
		SDL_Quit();
		return 1;
	}

	/*********************
	 * Background Drawing
	 *********************/
	//How many tiles are needed to fill the screen
	const int xTiles = SCREEN_WIDTH / TILE_SIZE;
	const int yTiles = SCREEN_HEIGHT / TILE_SIZE;

	//Draw tiles by calculating their positions
	for (int i = 0; i < xTiles * yTiles; i++) {
		int x = i % xTiles;
		int y = i / xTiles;
		//Scale coordinates to pixels by multiplying by tile size
		renderTexture(background, ren, x * TILE_SIZE, y * TILE_SIZE,
			TILE_SIZE, TILE_SIZE);
	}

	/*********************
	 * Foreground Drawing
	 *********************/
	int iW, iH;
	SDL_QueryTexture(image, NULL, NULL, &iW, &iH);
	int x = SCREEN_WIDTH / 2 - iW / 2;
	int y = SCREEN_HEIGHT / 2 - iH / 2;
	renderTexture(image, ren, x, y);

	SDL_RenderPresent(ren);
	SDL_Delay(5000);

	cleanup(background, image, ren, win);
	IMG_Quit();
	SDL_Quit();

	return 0;
}
