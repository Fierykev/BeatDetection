#define _USE_MATH_DEFINES

#include "Keyboard.h"
#include <math.h>
#include <stdio.h>
#include "Shader.h"
#include "Image.h"
#include "Track.h"

#define TRACKW .5

#define TSCALE .1

#define TRACKONSCREEN 100

#define LANEOFFSET -.01

#define BEATSTTORACK 1.0/44100.0

#define SAMPLESIZE 1024.0

// the number of tracks before a new random is chosen

#define FLATDELMIN 10

#define FLATDELMAX 15

// down and up vars

#define INCDELMIN 10

#define INCDELMAX 30

#define INCMIN 5

#define INCMAX 20

// general track creation variables

#define TRANSITION .005

#define SLOPEMAX .2

#define SLOPEMIN .1

// variables for playing beats

#define PLAYPERIOD .1

const double BEATSIZE = (TRACKW - LANEOFFSET * 4.0) / 4.0;

double slope;

// beat verts

#define BEATVERTNUM 12

GLfloat beatverts[] =
{
	0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.916616,
	-2.000000, 0.000000, 0.916615,
	-2.000000, 0.000000, -0.000000,
	0.000000, 1.000000, 0.000000,
	-0.000000, 1.000000, 0.916616,
	-2.000000, 1.000000, 0.916615,
	-2.000000, 1.000000, -0.000000,
	-0.475383, 1.427691, 0.217872,
	-0.475384, 1.427691, 0.698744,
	-1.524616, 1.427691, 0.698743,
	-1.524616, 1.427691, 0.217872

};

#define BEATINDEXNUM 40

// beat indices

GLuint beatindices[] =
{
	0, 1, 2, 3,
	7, 6, 10, 11,
	0, 4, 5, 1,
	1, 5, 6, 2,
	2, 6, 7, 3,
	4, 0, 3, 7,
	8, 11, 10, 9,
	6, 5, 9, 10,
	4, 7, 11, 8,
	5, 4, 8, 9
};

enum TRACKTYPE {
	FLAT,
	DOWN,
	UP
};

/**
Constructor
*/

Track::Track()
{

}

/**
Destructor
*/

Track::~Track()
{
	
}

int random(int min, int max)
{
	return rand() % (max - min + 1) + min;
}

/**
Add track pieces
*/

void Track::TrackPieceGen(int pieces)
{
	for (int i = 0; i < pieces; i++)
	{
		if (loopnum == loopcount)
		{
			loopnum = 0;

			// pick a new random

			type = random(0, 2);

			switch (type)
			{
			case FLAT:

				loopcount = random(FLATDELMIN, FLATDELMAX);

				break;
			case DOWN:

				loopcount = random(INCDELMIN, INCDELMAX);

				slope = -random(INCMIN, INCMAX) / 100.0;

				break;
			case UP:

				loopcount = random(INCDELMIN, INCDELMAX);

				slope = random(INCMIN, INCMAX) / 100.0;

				break;
			}
		}

		switch (type)
		{
		case FLAT:

			// no change in yval

			break;
		case DOWN || UP:

			// ease into the down

			if (currentslope < slope)
			{
				// add a transition to the slope so it levels out

				currentslope + TRANSITION < slope ? currentslope += TRANSITION : currentslope = slope;
			}
			else if (currentslope > slope)
			{
				// add a transition to the slope so it levels out

				currentslope - TRANSITION > slope ? currentslope -= TRANSITION : currentslope = slope;
			}

			break;
		}

		// calculate the yval

		yval += currentslope;

		verts.push_back(Vector3f(TRACKW / 2.0, yval, -piecenum * TSCALE));

		// odd or even

		if (i % 2 != 0)
			texcoords.push_back(Vector2f(1, 1));
		else
			texcoords.push_back(Vector2f(1, 0));

		verts.push_back(Vector3f(-TRACKW / 2.0, yval, -piecenum * TSCALE));

		// odd or even

		if (i % 2 != 0)
			texcoords.push_back(Vector2f(0, 1));
		else
			texcoords.push_back(Vector2f(0, 0));

		// delete excess track in the front
		
		if (TRACKONSCREEN * 2 < verts.size())
			verts.erase(verts.begin());

		loopnum++;

		piecenum++;
	}
}

/**
Create a randomized track
*/

void Track::CreateTrack(long songlen)
{
	// load the track image

	trackimage = LoadImageGL("Images/Track.png");

	beatbarimage = LoadImageGL("Images/Beat Bar.png");

	// create the track

	indexlen = TRACKONSCREEN - 1;
	
	indices = new Vector4ui[indexlen];

	// build the track

	verts.push_back(Vector3f(TRACKW / 2.0, 0, 0));

	texcoords.push_back(Vector2f(1, 0));

	verts.push_back(Vector3f(-TRACKW / 2.0, 0, 0));

	texcoords.push_back(Vector2f(0, 0));

	TrackPieceGen(TRACKONSCREEN);

	// create indicies

	for (int i = 1; i < indexlen + 1; i++)
		indices[i - 1] = Vector4ui(i * 2 - 2, i * 2 - 1, i * 2 + 1, i * 2);

	// create the shader if there is none

	if (trackshader == NULL)
	{
		trackshader = CreateShader("Shaders/Track/main.vs", "Shaders/Track/main.fs");

		// get position and color

		//trackPos = glGetAttribLocation(trackshader, "position");
		//trackColor = glGetAttribLocation(trackshader, "color");
	}
}

/**
Create beats on track
*/

void Track::CreateBeats()
{
	double SCALEVERTS = .05;

	// create the shader if there is none

	if (beatshader == NULL)
		beatshader = CreateShader("Shaders/Beats/main.vs", "Shaders/Beats/main.fs");
	
	// create beat mesh

	beatList = glGenLists(1);
	glNewList(beatList, GL_COMPILE);

	// center of figure is front left
	
	glBegin(GL_QUADS);

	Vector3f verts[4]; // tmp storage for verts

	Vector3f normal; // tmp storage for normals

	for (int i = 0; i < BEATINDEXNUM; i+=4)
	{
		//glVertex3f(beatverts[beatindices[i] * 3] * SCALEVERTS, beatverts[beatindices[i] * 3 + 1] * SCALEVERTS, beatverts[beatindices[i] * 3 + 2] * SCALEVERTS);
	
		for (int j = 0; j < 4; j++)
			verts[j] = Vector3f(beatverts[beatindices[i + j] * 3] * SCALEVERTS, beatverts[beatindices[i + j] * 3 + 1] * SCALEVERTS, beatverts[beatindices[i + j] * 3 + 2] * SCALEVERTS);

		findNormals(normal, verts[0], verts[1], verts[2], verts[3]);

		glNormal3f(normal.x, normal.y, normal.z);

		for (int j = 0; j < 4; j++)
			glVertex3f(verts[j].x, verts[j].y, verts[j].z);
	}
	glEnd();

	glEndList();
}

/**
Find the y position at a z coordinate
*/

Vector3f Track::findY(GLfloat z, GLfloat abovetrack)
{
	Vector3f campos;

	double position = abs(z - verts[0].z) / TSCALE;

	double slopey = (verts[(int)(floor(position) + 1) * 2].y - verts[(int)floor(position) * 2].y);

	double slopex = TSCALE;

	double slope = slopey / slopex;
	
	// calculate the position

	double ypos = verts[(int)floor(position) * 2].y + (verts[(int)floor(position) * 2].z - z) * slope;

	// find perpendicular slope

	double angle = atan2(slopex, slopey);
	//printf("%f, %f, %f\n", angle, slope, slopey);
	campos.x = 0;
	
	campos.y = ypos + abovetrack * sin(angle);

	campos.z = z + abovetrack * cos(angle);

	return campos;
}

/**
Find the y position and rotation at a z coordinate for beats
Lane is between 1 and 4
*/

Beatdraw Track::findYBeat(GLfloat z, int lane)
{
	Beatdraw posrot;

	Vector3f campos;

	double position = abs(z - verts[0].z) / TSCALE;

	double slopey = (verts[(int)(floor(position) + 1) * 2].y - verts[(int)floor(position) * 2].y);

	double slopex = TSCALE;

	double slope = slopey / slopex;

	// calculate the position

	double ypos = verts[(int)floor(position) * 2].y + (verts[(int)floor(position) * 2].z - z) * slope;
	
	posrot.rot = atan2(slopey, slopex) * 180.0 / M_PI;

	posrot.x = lane * LANEOFFSET + (lane - 1) * BEATSIZE - (LANEOFFSET + BEATSIZE);
	
	posrot.y = ypos;

	posrot.z = z;

	posrot.lane = lane - 1;

	return posrot;
}

/*Update methods*/

/**
Update the track by adding to it and deleting portions.
*/

void Track::UpdateTrack(GLfloat z)
{
	while (z + TSCALE * 5 < verts.at(0).z)
		TrackPieceGen(1);
}

/*Draw methods*/

/**
Draw the track
*/

void Track::DrawTrack()
{
	BindShader(trackshader);
	
	glEnable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, trackimage);

	glActiveTexture(GL_TEXTURE1);

	glBindTexture(GL_TEXTURE_2D, beatbarimage);

	// state the texture which should be used

	GLuint texUniformLocation = glGetUniformLocation(trackshader, "texture0");

	glUniform1i(texUniformLocation, 0);

	texUniformLocation = glGetUniformLocation(trackshader, "texture1");

	glUniform1i(texUniformLocation, 1);

	// set the lanes that should be colored
	
	float lanepass[4];

	for (int i = 0; i < NUMLANES; i++)
		lanepass[i] = keys[LANE_KEY[i]] ? 1 : 0;
	
	texUniformLocation = glGetUniformLocation(trackshader, "lane");
	
	glUniform1fv(texUniformLocation, NUMLANES, lanepass);

	texUniformLocation = glGetUniformLocation(trackshader, "error");

	glUniform1fv(texUniformLocation, NUMLANES, beaterror);

	//lanepress

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_INDEX_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, getVerts());
	glTexCoordPointer(2, GL_FLOAT, 0, getTexCoords());
	glDrawElements(GL_QUADS, getIndexLen(), GL_UNSIGNED_INT, getIndices());

	glDisable(GL_TEXTURE_2D);
	
	// unbind the buffer

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/**
Draw the beats
*/

void Track::DrawBeats(GLfloat z, GLfloat camlag, XAudio2* audio)
{
	// find the position and rotation for all of the beats

	Vector4f posrot;

	Vector3f pos;

	vector<Beat>* beats = audio->getBeats();

	// bind the shader

	BindShader(beatshader);
	
	// enable transparency

	glEnable(GL_BLEND);

	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	
	// delete old beats

	while (beatposrot.size() != 0 && beatposrot.at(0).z - z >= 0)
		beatposrot.erase(beatposrot.begin());

	// add new beats
	
	while (beats->size() != lastbeat && -(float)beats->at(lastbeat).beat * SAMPLESIZE * BEATSTTORACK > verts.at(verts.size() - 1).z)
	{
		beatposrot.push_back(findYBeat(-(float)beats->at(lastbeat).beat * SAMPLESIZE * BEATSTTORACK, beats->at(lastbeat).lane));

		lastbeat++;
	}

	// setup note checker
	
	bool keylane[NUMLANES] = { false };

	glEnable(GL_COLOR);

	// draw the beats

	for (int i = 0; i < beatposrot.size(); i++)
	{
		glPushMatrix();
		
		if (beatposrot.at(i).z - z < camlag) // area where notes cannot be played
		{

		}
		else if (beatposrot.at(i).z - z < camlag + PLAYPERIOD) // area where notes can be played
		{
			/*
			if (!(beatposrot.at(i).flag & CORRECT)) // only check for points if beat is not already looked at
			{

			}*/

			if (keys[LANE_KEY[beatposrot.at(i).lane]]) // pressed
				beatposrot.at(i).flag |= CORRECT; // turn on correct flag

			keylane[beatposrot.at(i).lane] = true;
		}
		else if (!(beatposrot.at(i).flag & CORRECT)) // check if beat is missed by time
			beatposrot.at(i).flag |= INCORRECT;

		// look at flags

		if (beatposrot.at(i).flag & INCORRECT)
			glColor4f(incorrectcolor.r, incorrectcolor.g, incorrectcolor.b, incorrectcolor.a);
		else if (beatposrot.at(i).flag & CORRECT)
			glColor4f(correctcolor.r, correctcolor.g, correctcolor.b, correctcolor.a);
		else
			glColor4f(trackcolor[beatposrot.at(i).lane].r, trackcolor[beatposrot.at(i).lane].g, trackcolor[beatposrot.at(i).lane].b, trackcolor[beatposrot.at(i).lane].a);

		glTranslatef(beatposrot.at(i).x, beatposrot.at(i).y, beatposrot.at(i).z);
		glRotatef(beatposrot.at(i).rot, 1.0, 0, 0);
		glCallList(beatList);

		glPopMatrix();
	}

	// check if too many notes were played at once

	for (int i = 0; i < NUMLANES; i++)
	{
		if (keys[LANE_KEY[i]] && !keylane[i])
		{
			// register the error lane

			beaterror[i] = 1.0;

			// release all lane keys

			//for (int j = 0; j < NUMLANES; j++)
				//keyUp(LANE_KEY[j]);

			// register the error


		}
		else
			beaterror[i] = .0; // no error in user input
	}

	glDisable(GL_COLOR);

	// reset the colors to white

	glColor4f(1, 1, 1, 1);

	// disable transparency

	glDisable(GL_BLEND);
}

/**
Convert an audio sample position to a track position
*/

GLfloat Track::toTrackPos(GLfloat pos)
{
	return pos * BEATSTTORACK;
}

/**
Get vertecies
*/

GLfloat* Track::getVerts()
{
	return (GLfloat*)&verts[0];
}

/**
Get texture coordinates
*/

GLfloat* Track::getTexCoords()
{
	return (GLfloat*)&texcoords[0];
}

/**
Get indices
*/

GLuint* Track::getIndices()
{
	return (GLuint*)indices;
}

/**
Get the index length
*/

double Track::getIndexLen()
{
	return indexlen * 4;
}

/**
Get track image
*/

GLuint Track::getTrackImage()
{
	return trackimage;
}