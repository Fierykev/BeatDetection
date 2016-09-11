#include <stdio.h>
#include "Keyboard.h"
#include "Shader.h"
#include "glut.h"
#include "Audio.h"
#include "Track.h"
#include "Camera.h"
#include "Image.h"
#include "3D.h"
#include "2D.h"
#include <ctime>

#include <iostream>

#define WINDOW_W 960.0

#define WINDOW_H 540.0

clock_t fps;

char lanekey[] = { 'a', 's', 'd', 'f' };

XAudio2 audio;

Track track;

Camera camera;

long long frames;

float fps_t;

void init(void)
{
	glViewport(0, 0, WINDOW_W, WINDOW_H);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0f, WINDOW_W / WINDOW_H, .1f, 10000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glShadeModel(GL_SMOOTH);
	glClearColor(.0, .0, .0, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	setup3D(WINDOW_W, WINDOW_H);

	// camera
	
	camera.UpdateZ(&track, &audio);

	// update track

	track.UpdateTrack(camera.getCamposZ());

	// update camera

	camera.UpdatePos(&track);

	// draw beats

	track.DrawBeats(camera.getCamposZ(), camera.getCameraLag(), &audio);
	
	// draw the track

	track.DrawTrack();

	// draw 2D

	setup2D(WINDOW_W, WINDOW_H);

	glColor3f(1.0f, 0.0f, 0.0f); // set the text color

	glRasterPos2i(10, 10); // set the text position

	float seconds = ((std::clock() - fps) / (double)CLOCKS_PER_SEC); // calculate seconds passed

	// calculate fps if it has been a second

	if (seconds > 1.0)
	{
		fps_t = frames / seconds;

		fps = clock();

		frames = 0;
	}

	string s = to_string(fps_t); // draw fps

	drawString(s, GLUT_BITMAP_9_BY_15);

	frames++; // next frame

	glPopMatrix();
	
	glutSwapBuffers();
	glutPostRedisplay();
}

void SpecialKeyCallback(int key, int x, int y)
{
	/*
	switch (key) {
	case GLUT_KEY_LEFT:
		roty -= 5;
		break;
	case GLUT_KEY_RIGHT:
		roty += 5;
		break;
	case GLUT_KEY_DOWN:
		rot += 5;
		break;
	case GLUT_KEY_UP:
		rot -= 5;
		break;
	default:
		break;
	}
	*/

	//glutPostRedisplay();
}

void KeyDownCallback(unsigned char key, int x, int y)
{
	keyDown(key);

	/*
	for (int i = 0; i < NUMLANES; i++)
	{
		if (lanekey[i] == key)
			lanepress |= 1 << i; // set the bit to true
		else
			lanepress &= ~(1 << i); // set the bit ot false
	}
	*/
	//glutPostRedisplay();
}

void KeyUpCallback(unsigned char key, int x, int y)
{
	keyUp(key);
}

int main(int argc, char** argv)
{
	// random seed set to time

	srand(time(NULL));

	// init Devil

	initDevil();

	// create XAudio2

	audio.LoadWaveX(L"Audio/Linkin_Park Mono 16.wav");
	//Linkin_Park Mono 16
	// create display

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WINDOW_W, WINDOW_H);
	//glutInitWindowPosition(100, 100);
	glutCreateWindow("OpenGL Test");
	glutDisplayFunc(display);
	
	// init Glew

	initGlew();
	
	// create track

	track.CreateTrack(audio.getLength());
	
	track.CreateBeats();

	// play the song

	audio.PlayWaveX();
	
	glutSpecialFunc(SpecialKeyCallback);
	glutKeyboardFunc(KeyDownCallback);
	glutKeyboardUpFunc(KeyUpCallback);

	fps = clock();

	glutMainLoop();

	return 0;
}