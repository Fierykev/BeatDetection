#include <string>
#include <fstream>
#include "glew.h"
#include "glut.h"

using namespace std;

/**
Initialize Glew
*/

void initGlew()
{
	GLenum err = glewInit();

	if (err != 0)
	{
		printf("Cannot initialize Glew");

		exit(1);
	}
}

/**
Load the shader
*/

string* LoadFile(string name)
{
	ifstream file(name);
	string* output = new string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
	return output;
}

/**
Create the shader
*/

GLuint CreateShader(string vsfilename, string fsfilename)
{
	string* vs, *fs;

	vs = LoadFile(vsfilename);

	fs = LoadFile(fsfilename);

	const char* vfile = vs->c_str();

	const char* ffile = fs->c_str();

	// create the shader for the track
	
	GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vshader, 1, (const char **)&vfile, NULL);
	glCompileShader(vshader);

	free(vs);

	GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshader, 1, (const char **)&ffile, NULL);
	glCompileShader(fshader);

	free(fs);

	// create the shader
	
	GLuint shader = glCreateProgram();
	glAttachShader(shader, vshader);
	glAttachShader(shader, fshader);
	glLinkProgram(shader);
	
	GLint link;

	glGetProgramiv(shader, GL_LINK_STATUS, &link);
	
	// Exit if the program couldn't be linked correctly
	if (!link)
	{
		printf("Could not link shader\n");
		exit(EXIT_FAILURE);
	}
	
	return shader;
}

/**
Bind the shader for OpenGl to use
*/

void BindShader(GLuint shader)
{
	glUseProgram(shader);
}