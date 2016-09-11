#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <string>

using namespace std;

void initDevil()
{
	ilInit(); // init devil.  must be done once
}

GLuint LoadImageGL(string file)
{
	unsigned int image;

	// create image

	ilGenImages(1, &image);

	// bind the image

	ilBindImage(image);

	// tell devil that we are using OpenGL format for images

	ilEnable(IL_ORIGIN_SET);
	
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	// load the image

	if (!ilLoadImage((ILstring)file.c_str()))
	{
		ilDeleteImages(1, &image);

		printf("Error Loading %s.", file);

		exit(1);
	}

	// convert the image to RGBA

	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	// create OpenGL image

	GLuint texture;

	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH),
		ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE,
		ilGetData());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// delete the Devil Image

	//ilDeleteImages(1, &image);

	// return the texture id

	return texture;
}