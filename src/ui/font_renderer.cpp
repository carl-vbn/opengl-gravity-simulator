#include "font_renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace ui {
	namespace fontRendering {
		GLuint FontTextureID;
		short int* FontData;

		int AtlasWidth, AtlasHeight;

		void init() {
			// Load the font atlas as texture
			std::cout << "Loading font atlas..." << std::endl;
			int nrChannels;
			unsigned char* data = stbi_load("fonts\\Comfortaa.png", &AtlasWidth, &AtlasHeight, &nrChannels, 0);

			glGenTextures(1, &FontTextureID);
			glBindTexture(GL_TEXTURE_2D, FontTextureID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, AtlasWidth, AtlasHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
			std::cout << "Font atlas loaded." << std::endl;

			// Load font data
			std::cout << "Loading font data..." << std::endl;
			std::ifstream file("fonts\\Comfortaa.fnt");
			FontData = new short int[128*7];

			std::string line;
			while (std::getline(file, line)) {
				if (line.rfind("chars count=") == 0) {
					int charCount = std::stoi(line.substr(12, 3));
					std::cout << "Font contains " << charCount << " characters." << std::endl;
				}
				else if (line.rfind("char id=") == 0) {
					int charId = std::stoi(line.substr(8, 3));
					
					int x = std::stoi(line.substr(line.rfind("x=") + 2, 3));
					int y = std::stoi(line.substr(line.rfind("y=") + 2, 3));
					int width = std::stoi(line.substr(line.rfind("width=") + 6, 3));
					int height = std::stoi(line.substr(line.rfind("height=") + 7, 3));
					int xoffset = std::stoi(line.substr(line.rfind("xoffset=") + 8, 3));
					int yoffset = std::stoi(line.substr(line.rfind("yoffset=") + 8, 3));
					int xadvance = std::stoi(line.substr(line.rfind("xadvance=") + 9, 3));

					FontData[charId * 7 + 0] = x;
					FontData[charId * 7 + 1] = y;
					FontData[charId * 7 + 2] = width;
					FontData[charId * 7 + 3] = height;
					FontData[charId * 7 + 4] = xoffset;
					FontData[charId * 7 + 5] = yoffset;
					FontData[charId * 7 + 6] = xadvance;
				}
			}
		}

		void drawText(std::string text, float x, float y, float scale, Color color, bool centerHorizontally, bool centerVertically) {
			int strLength = text.length();
			float aspectRatio = (float)renderer::windowWidth / renderer::windowHeight;
			int cursorX = 0;

			if (centerHorizontally || centerVertically) {
				// Calculate the dimensions of the text
				float width = 0;
				float highest = -1;
				float lowest = 1;

				for (int c = 0; c < strLength; c++) {
					char chr = text.at(c);

					// Extract character data from font
					int chrX = FontData[chr * 7 + 0];
					int chrY = FontData[chr * 7 + 1];
					int chrWidth = FontData[chr * 7 + 2];
					int chrHeight = FontData[chr * 7 + 3];
					int chrXOffset = FontData[chr * 7 + 4];
					int chrYOffset = FontData[chr * 7 + 5];
					int chrXAdvance = FontData[chr * 7 + 6];
					
					width += (float)chrXAdvance / AtlasWidth * scale;

					float vertexOffsetY = (float)chrYOffset / -AtlasHeight * aspectRatio;
					float relativeHeight = (float)chrHeight / AtlasHeight;

					highest = std::fmaxf(highest, vertexOffsetY * scale);
					lowest = std::fminf(lowest, (float)(vertexOffsetY - relativeHeight * aspectRatio) * scale);
				}

				if (centerHorizontally) x -= width / 2;
				if (centerVertically) y -= (highest + lowest) / 2;

			}

			glBindTexture(GL_TEXTURE_2D, FontTextureID);
			glUniform1i(renderer::uiShader.UseTextureUniformID, 1);
			glBegin(GL_QUADS); // WARNING GL_QUADS IS DEPREACTED AND SHOULD NOT BE USED. TODO: Use GL_TRIANGLES instead
			glVertexAttrib3d(1, color.red, color.green, color.blue);

			for (int c = 0; c < strLength; c++) {
				char chr = text.at(c);

				// Extract character data from font
				int chrX = FontData[chr * 7 + 0];
				int chrY = FontData[chr * 7 + 1];
				int chrWidth = FontData[chr * 7 + 2];
				int chrHeight = FontData[chr * 7 + 3];
				int chrXOffset = FontData[chr * 7 + 4];
				int chrYOffset = FontData[chr * 7 + 5];
				int chrXAdvance = FontData[chr * 7 + 6];

				float relativeX = (float)chrX / AtlasWidth;
				float relativeY = (float)chrY / AtlasHeight;
				float relativeWidth = (float)chrWidth / AtlasWidth;
				float relativeHeight = (float)chrHeight / AtlasHeight;

				float vertexOffsetX = (float)(cursorX + chrXOffset) / AtlasWidth;
				float vertexOffsetY = (float)chrYOffset / -AtlasHeight * aspectRatio;

				glVertexAttrib2f(2, relativeX, relativeY);
				glVertex2f(vertexOffsetX * scale + x, vertexOffsetY * scale + y);

				glVertexAttrib2f(2, relativeX, relativeY + relativeHeight);
				glVertex2f(vertexOffsetX * scale + x, (vertexOffsetY-relativeHeight * aspectRatio) * scale + y);

				glVertexAttrib2f(2, relativeX + relativeWidth, relativeY + relativeHeight);
				glVertex2f((vertexOffsetX+relativeWidth) * scale + x, (vertexOffsetY - relativeHeight * aspectRatio) * scale + y);

				glVertexAttrib2f(2, relativeX + relativeWidth, relativeY);
				glVertex2f((vertexOffsetX+relativeWidth) * scale + x, vertexOffsetY * scale + y);

				cursorX += chrXAdvance;
			}

			glEnd();
			glUniform1i(renderer::uiShader.UseTextureUniformID, 0);
		}

		void dispose() {
			glDeleteTextures(1, &FontTextureID);
			delete[] FontData;
		}
	}
}