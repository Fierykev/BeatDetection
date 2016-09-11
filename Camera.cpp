#include "Camera.h"
#include "Audio.h"
#include <cstdio>

#define CAMLAG .6

#define CAMY .5

/**
Constructor
*/
Camera::Camera()
{
	camposz = 0;
}

/**
Destructor
*/

Camera::~Camera()
{

}

/**
Update the Z position
*/

void Camera::UpdateZ(Track* track, XAudio2* audio)
{
	camposz = -track->toTrackPos((float)audio->getSamplesPlayed()) + CAMLAG; // lag behind where the beat is
}

/**
Update the camera position
*/

void Camera::UpdatePos(Track* track)
{	
	Vector3f campos = track->findY(camposz, CAMY); // the camera position

	Vector3f centerpos = track->findY(camposz - 1.0, 0); // look at where the beat will happen

	gluLookAt(campos.x, campos.y, campos.z, centerpos.x, centerpos.y, centerpos.z, 0, 1, 0);
}

/**
Get the z position of movement
*/

GLfloat Camera::getCamposZ()
{
	return camposz;
}

/**
Get the camera lag
*/

GLfloat Camera::getCameraLag()
{
	return -CAMLAG;
}