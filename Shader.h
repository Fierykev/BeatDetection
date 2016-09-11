#ifndef SHADER_H
#define SHADER_H

#include <string>
#include "glew.h"
#include "glut.h"

void initGlew();

GLuint CreateShader(std::string vsfilename, std::string fsfilename);

void BindShader(GLuint shader);

#endif