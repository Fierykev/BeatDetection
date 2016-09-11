#ifndef NORMALS_H
#define NORMALS_H

#include "glut.h"

struct Vector3f
{
	GLfloat x, y, z;

	Vector3f()
	{

	}

	Vector3f(GLfloat px, GLfloat py, GLfloat pz)
	{
		x = px;

		y = py;

		z = pz;
	}
};

struct Vector2f
{
	GLfloat x, y;

	Vector2f()
	{

	}

	Vector2f(GLfloat px, GLfloat py)
	{
		x = px;

		y = py;
	}
};

struct Vector4ui
{
	GLuint w, x, y, z;

	Vector4ui()
	{

	}

	Vector4ui(GLuint pw, GLuint px, GLuint py, GLuint pz)
	{
		w = pw;

		x = px;

		y = py;

		z = pz;
	}
};

struct Vector4f
{
	GLfloat w, x, y, z;

	Vector4f()
	{

	}

	Vector4f(GLfloat pw, GLfloat px, GLfloat py, GLfloat pz)
	{
		w = pw;

		x = px;

		y = py;

		z = pz;
	}
};

struct RGBA
{
	GLfloat r, g, b, a;

	RGBA()
	{

	}

	RGBA(GLfloat pr, GLfloat pg, GLfloat pb, GLfloat pa)
	{
		r = pr;

		g = pg;

		b = pb;

		a = pa;
	}
};

void findNormals(Vector3f& normal, Vector3f a, Vector3f b, Vector3f c, Vector3f d);

#endif