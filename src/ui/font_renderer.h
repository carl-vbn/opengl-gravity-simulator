#pragma once

#include <GL/glew.h>
#include <string>
#include <iostream>
#include <fstream>

#include "../rendering/renderer.h"

namespace ui {
	namespace fontRendering {
		extern GLuint FontTextureID;
		extern int AtlasWidth, AtlasHeight;

		extern short int* FontData; // Array containing the font data of every character from the .fnt file

		void init();
		void dispose();
		void drawText(std::string text, float x, float y, float scale, Color color, bool centerHorizontally= false, bool centerVertically=false);
	}
}