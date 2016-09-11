#ifndef CAMERA_H
#define CAMERA_H

#include "glut.h"
#include "Track.h"

class Camera
{
public:
	Camera();

	~Camera();

	void UpdateZ(Track* track, XAudio2* audio);

	void UpdatePos(Track* track);

	GLfloat getCamposZ();

	GLfloat getCameraLag();

private:

	GLfloat camposz;
};

#endif