#ifndef RES_PATH_H
#define RES_PATH_H

#include <iostream>
#include <SDL.h>
#include <string>

/**
 * Get the resource path for resoures located under res/
 * assuming the project directory is:
 * bin/
 * 	executables
 * res/
 * 	Lesson0
 * 	Lesson1
 * 	...
 *
 * Path will return as __________
 */
std::string getResourcePath(const std::string &subDir = "") {

//Paths separated by \ in Windows, / elsewhere
#ifdef _WIN32
	const char PATH_SEP = '\\';
#else
	const char PATH_SEP = '/';
#endif

	//No need to call this more than once, keep it static
	//Holds path up until res/
	static std::string baseRes;

	if (baseRes.empty()) {
		char* basePath = SDL_GetBasePath();

		//SDL_GetBasePath() may return NULL if an error occured
		if (basePath) {
			//We own the basePath pointer, so we must free it
			baseRes = basePath;
			SDL_free(basePath);
		} else {
			std::cerr << "Error getting resource path: " << SDL_GetError() << std::endl;
			return "";
		}
		// Replace trailing bin/ with res/
		size_t pos = baseRes.rfind("bin");
		baseRes = baseRes.substr(0, pos) + "res" + PATH_SEP;
	}

	//Return "res/" or "res/{subDir}"
	return subDir.empty() ? (baseRes) : (baseRes + subDir + PATH_SEP);
}

#endif
