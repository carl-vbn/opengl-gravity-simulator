#pragma once

#include "universe.h"
#include <fstream>

Universe* loadScene(const char* filePath);
void saveScene(Universe* universe, const char* filePath);