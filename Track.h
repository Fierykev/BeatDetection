#ifndef TRACK_H
#define TRACK_H

#include "Audio.h"
#include "Mesh.h"
#include "glut.h"

#define NUMLANES 4

#define INCORRECT 1 << 0
#define CORRECT 1 << 1

struct Beatdraw
{
	GLfloat rot, x, y, z;

	int lane;

	int flag;

	Beatdraw()
	{
		flag = 0; // no flag
	}

	Beatdraw(GLfloat prot, GLfloat px, GLfloat py, GLfloat pz, int plane, int pflag)
	{
		rot = prot;

		x = px;

		y = py;

		z = pz;

		lane = plane;

		flag = 0;
	}
};

const RGBA trackcolor[] = {
	RGBA(1, 0, 1, 1),
	RGBA(0, 1, 1, 1),
	RGBA(0, 0, 1, 1),
	RGBA(1, 1, 0, 1)
};

const RGBA incorrectcolor = RGBA(.5, 0, 0, .1);

const RGBA correctcolor = RGBA(0, .5, 0, .1);

class Track
{
public:

	Track();

	~Track();

	void TrackPieceGen(int pieces);

	void UpdateTrack(GLfloat z);

	void CreateTrack(long songlen);

	void CreateBeats();

	Vector3f findY(GLfloat z, GLfloat abovetrack);

	void DrawTrack();

	void DrawBeats(GLfloat z, GLfloat camlag, XAudio2* audio);

	GLfloat* getVerts();

	GLfloat* getTexCoords();

	GLuint* getIndices();

	double getIndexLen();

	GLuint getTrackImage();

	GLfloat toTrackPos(GLfloat pos);

private:

	// methods

	Beatdraw findYBeat(GLfloat z, int lane);

	// compiled beat object

	GLuint beatList;

	// beat position and rotation

	vector<Beatdraw> beatposrot;

	// track verts, texcoord, indices, and length of indices

	vector<Vector3f> verts;

	vector<Vector2f> texcoords;

	Vector4ui* indices;

	long indexlen;

	// images for track and beat bar

	GLuint trackimage, beatbarimage;

	// Track construction specific vars

	int loopcount = 0; // used to track how many loops between random picks

	int loopnum = 0; // used to track how many loops since the last random pick

	double yval = 0; // the random y value of the track

	int type = 0; // the current type of track that is chosen

	double slope = 0, currentslope = 0; // the slope of the track

	long piecenum = 0; // the number of pieces of track created so far

	// Beat consturction specific var

	int lastbeat = 0;

	// Beat color specific vars

	float beaterror[NUMLANES]; // user entered incorrect key
	
	// shaders

	GLuint trackshader = NULL, beatshader = NULL;
};

#endif