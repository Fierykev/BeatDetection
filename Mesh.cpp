#include "Mesh.h"
#include <math.h>

/**
Find normals for quad
*/

void findNormals(Vector3f& normal, Vector3f a, Vector3f b, Vector3f c, Vector3f d)
{
	// formula for quad normals: (c - a) * (d - b)

	// use the cross product

	Vector3f v1 = Vector3f(c.x - a.x, c.y - a.y, c.z - a.z);

	Vector3f v2 = Vector3f(d.x - b.x, d.y - b.y, d.z - b.z);

	// v1 * v2

	normal.x = v1.y * v2.z - v1.z * v2.y;

	normal.y = v2.x * v1.z - v2.z * v1.x;

	normal.z = v1.x * v2.y - v1.y * v2.x;

	// check if needs to be normalized

	// calculate the normal distribution

	float len = normal.x * normal.x + normal.y * normal.y + normal.z * normal.z;

	// normalize if needed

	if (len != 1)
	{
		len = sqrt(len);

		normal.x /= len;

		normal.y /= len;

		normal.z /= len;
	}
}