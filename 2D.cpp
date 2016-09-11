#include <string>
#include <glut.h>

using namespace std;

/**
Setup 2D render
*/

void setup2D(double width, double height)
{
	glClear(GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);

	glPushMatrix();
	glLoadIdentity();

	gluOrtho2D(0.0, width, 0.0, height);
	glMatrixMode(GL_MODELVIEW);
}

/**
Iterate through the string and draw each character
*/

void drawString(string& s, void* font)
{
	char c;

	for (string::iterator i = s.begin(); i != s.end(); ++i)
	{
		c = *i;

		glutBitmapCharacter(font, c);
	}
}