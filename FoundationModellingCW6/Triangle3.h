#ifndef TRIANGLE_3_H
#define TRIANGLE_3_H

#include "Vec3.h"

class Triangle3
{

private:

	Vec3<float> m_vertices[3];
	Vec3<float> m_normals[3];

public:

	Triangle3() {};

	Triangle3(Vec3<float> vertices[3], Vec3<float> normals[3]);

	Vec3<float> getVertex(int i) const { return m_vertices[i]; }
	const Vec3<float>& getNormal(int i) const { return m_normals[i]; }







};

#endif