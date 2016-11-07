#include <iostream>
#include <SDL.h>
#include <string>

#include "res_path.h"
#include "cleanup.h"

//Screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

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
 * Loads a BMP image into a texture on the rendering device.
 *
 * @param  file The BMP image file to load.
 * @param  ren  The renderer to load the texture onto.
 * @return      The loaded texture, or nullptr if something went wrong.
 */
SDL_Texture* loadTexture(const std::string& file, SDL_Renderer* ren) {
	SDL_Texture* texture = nullptr;
	SDL_Surface* loadedImage = SDL_LoadBMP(file.c_str());

	if (loadedImage != nullptr) {
		texture = SDL_CreateTextureFromSurface(ren, loadedImage);
		SDL_FreeSurface(loadedImage);
		//Make sure converting went well
		if (texture == nullptr) {
			logSDLError(std::cout, "CreateTextureFromSurface");
		}
	} else {
		logSDLError(std::cout, "LoadBMP");
	}

	return texture;
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
	//Setup destination rectangle at given (x,y)
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	//Query the texture to get its width and height to use
	SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
	SDL_RenderCopy(ren, tex, NULL, &dst);
}

int main (int argc, char** argv) {

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		logSDLError(std::cout, "SDL_Init");
		return 1;
	}

	/* Window initialization */
	SDL_Window* win = SDL_CreateWindow("Lesson 2", 100, 100,
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
	const std::string resPath = getResourcePath("Lesson2");
	SDL_Texture* background = loadTexture(resPath + "background.bmp", ren);
	SDL_Texture* image = loadTexture(resPath + "image.bmp", ren);
	if (background == nullptr || image == nullptr) {
		cleanup(background, image, ren, win);
		SDL_Quit();
		return 1;
	}

	SDL_RenderClear(ren);

	/* Tiled Background Drawing */
	int bW, bH;
	SDL_QueryTexture(background, NULL, NULL, &bW, &bH);
	renderTexture(background, ren, 0, 0);
	renderTexture(background, ren, bW, 0);
	renderTexture(background, ren, 0, bH);
	renderTexture(background, ren, bW, bH);

	/* Image Drawing */
	int iW, iH;
	SDL_QueryTexture(image, NULL, NULL, &iW, &iH);
	int x = SCREEN_WIDTH / 2 - iW / 2;
	int y = SCREEN_HEIGHT / 2 - iH / 2;
	renderTexture(image, ren, x, y);

	SDL_RenderPresent(ren);
	SDL_Delay(1000);

	cleanup(background, image, ren, win);
	SDL_Quit();

	return 0;
}
