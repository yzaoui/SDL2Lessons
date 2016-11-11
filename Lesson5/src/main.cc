#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
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
 * Draw an SDL_Texture to an SDL_Renderer at some destination
 * rectangle, with optional clipping rectangle.
 *
 * @param tex  The source texture we want to draw.
 * @param ren  The renderer we want to draw to.
 * @param dst  The destination rectangle to render the texture to.
 * @param clip The sub-section of the texture to draw, default nullptr
 *             	  draws the entire texture.
 */
void renderTexture(SDL_Texture* tex, SDL_Renderer* ren, SDL_Rect dst,
		SDL_Rect* clip = nullptr) {
	SDL_RenderCopy(ren, tex, clip, &dst);
}

/**
 * Draw an SDL_Texture to an SDL_Renderer at (x,y), preserving
 * the texture's width and height and optionally taking a clip of
 * the texture. If a clip is passed, the clip's width and height
 * will be used instead of the texture's.
 *
 * @param tex  The source texture we want to draw.
 * @param ren  The renderer we want to draw to.
 * @param x    The x coordinate to draw to.
 * @param y    The y coordinate to draw to.
 * @param clip The sub-section of the texture to draw, default nullptr
 *                draws the entire texture.
 */
void renderTexture(SDL_Texture* tex, SDL_Renderer* ren, const int x, const int y,
		SDL_Rect* clip = nullptr) {
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;

	if (clip != nullptr) {
		dst.w = clip->w;
		dst.h = clip->h;
	} else {
		SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
	}

	renderTexture(tex, ren, dst, clip);
}

int main (int argc, char** argv) {
	/******************************
	 * Initialization
	 ******************************/
	/* SDL initialization */
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		logSDLError(std::cout, "SDL_Init");
		return 1;
	}
	/* Window initialization */
	SDL_Window* win = SDL_CreateWindow("Lesson 5", 100, 100,
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
	const std::string resPath = getResourcePath("Lesson5");
	SDL_Texture* image = loadTexture(resPath + "image.png", ren);
	if (image == nullptr) {
		cleanup(image, ren, win);
		SDL_Quit();
		return 1;
	}

	/******************************
	 * Image Drawing
	 ******************************/
	//Image width & height
	int imageW, imageH; //These need to be const somehow
	SDL_QueryTexture(image, NULL, NULL, &imageW, &imageH);
	//Clip width & height
	const int tileW = 100;
	const int tileH = 100;
	//Number of tiles
	const int xTiles = imageW / tileW;
	const int yTiles = imageH / tileH;
	const int totalTiles = xTiles * yTiles;

	//Build clips
	SDL_Rect clips[totalTiles];
	for (int i = 0; i < totalTiles; i++) {
		clips[i].x = i / yTiles * tileW;
		clips[i].y = i % yTiles * tileH;
		clips[i].w = tileW;
		clips[i].h = tileH;
	}

	int useClip = 0;

	//Screen grid numbers
	//Fill in extra space if screen is not a multiple of tilesize
	const int xScreenTiles = SCREEN_WIDTH / tileW
		+ (SCREEN_WIDTH % tileW != 0 ? 1 : 0);
	const int yScreenTiles = SCREEN_HEIGHT / tileH
		+ (SCREEN_HEIGHT % tileH != 0 ? 1 : 0);
	const int totalScreenTiles = xScreenTiles * yScreenTiles;

	/******************************
	 * Input Handling
	 ******************************/
	//Event structure
	SDL_Event e;
	//Track when to quit
	bool quit = false;

	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = true;
			} else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
					case SDLK_1:
					case SDLK_KP_1:
						useClip = 0;
						break;
					case SDLK_2:
					case SDLK_KP_2:
						useClip = 1;
						break;
					case SDLK_3:
					case SDLK_KP_3:
						useClip = 2;
						break;
					case SDLK_4:
					case SDLK_KP_4:
						useClip = 3;
						break;
					case SDLK_ESCAPE:
						quit = true;
						break;
				}
			}
		}
		//Render
		SDL_RenderClear(ren);
		//Draw the image
		for (int i = 0; i < totalScreenTiles; i++) {
			int x = i / yScreenTiles * tileW;
			int y = i % yScreenTiles * tileH;
			renderTexture(image, ren, x, y, &clips[useClip]);
		}
		//Update the screen
		SDL_RenderPresent(ren);
	}

	cleanup(image, ren, win);
	IMG_Quit();
	SDL_Quit();

	return 0;
}
