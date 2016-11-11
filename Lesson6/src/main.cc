#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
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

/**
 * Render the message to display to a texture for drawing.
 *
 * @param  message  The message to display.
 * @param  fontFile The font to use to render the text.
 * @param  color    The text color.
 * @param  fontSize The text size.
 * @param  renderer The renderer to load the texture in.
 * @return          An SDL_Texture containing the renderered message,
 *                     or nullptr if something went wrong.
 */
SDL_Texture* renderText(const std::string& message, const std::string& fontFile,
	SDL_Color color, int fontSize, SDL_Renderer* renderer) {
	//Open the font
	//**NOTE: It's very inefficient to constantly and open the font,
	//but it's okay for our purposes for now.
	TTF_Font* font = TTF_OpenFont(fontFile.c_str(), fontSize);
	if (font == nullptr) {
		logSDLError(std::cout, "TTF_OpenFont");
		return nullptr;
	}
	//TTF_RenderText returns a surface to render to, then load it into a texture
	SDL_Surface* surf = TTF_RenderText_Blended(font, message.c_str(), color);
	if (surf == nullptr) {
		TTF_CloseFont(font);
		logSDLError(std::cout, "TTF_RenderText");
		return nullptr;
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surf);
	if (texture == nullptr) {
		logSDLError(std::cout, "CreateTexture");
	}
	//Clean up the surface and font
	SDL_FreeSurface(surf);
	TTF_CloseFont(font);
	return texture;
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
	/* SDL_ttf initialization */
	if (TTF_Init() != 0) {
		logSDLError(std::cout, "TTF_Init");
		SDL_Quit();
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

	/* Texture initialization */
	const std::string resPath = getResourcePath("Lesson6");
	//Color is in RGBa format
	SDL_Color color = {255, 255, 255, 255};
	SDL_Texture* image = renderText("TTF fonts are neat!",
		resPath + "OpenSans-Regular.ttf", color, 64, ren);
	if (image == nullptr) {
		cleanup(ren, win);
		TTF_Quit();
		SDL_Quit();
		return 1;
	}

	int iW, iH;
	SDL_QueryTexture(image, NULL, NULL, &iW, &iH);
	const int x = SCREEN_WIDTH / 2 - iW / 2;
	const int y = SCREEN_HEIGHT / 2 - iH / 2;

	/******************************
	 * Event Loop / Text Drawing
	 ******************************/
	SDL_Event e;
	bool quit = false;

	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT ||
				(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
				quit = true;
			}
		}
		SDL_RenderClear(ren);
		//Text is drawn just like any other texture,
		//since it's been rendered to a texture.
		renderTexture(image, ren, x, y);
		SDL_RenderPresent(ren);
	}

	/******************************
	 * Clean up
	 ******************************/
	cleanup(image, ren, win);
	TTF_Quit();
	SDL_Quit();

	return 0;
}
