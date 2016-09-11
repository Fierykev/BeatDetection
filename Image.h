#ifndef IMAGE_H
#define IMAGE_H

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <string>

void initDevil();

GLuint LoadImageGL(std::string file);

#endif