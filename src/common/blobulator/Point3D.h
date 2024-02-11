//==== Blobulator. Copyright © 2004-2024, Ilya D. Rosenberg, All rights reserved. ====//
#pragma once
#include "utlvector.h"
#include <xmmintrin.h>

class Point3D {
public:

	Point3D();
	Point3D(float x, float y, float z);

	// blob monster
	Point3D(Vector vec) {
		p[0] = vec.x;
		p[1] = vec.y;
		p[2] = vec.z;
	}

	Point3D		operator*(float x) const;
	Point3D		operator+(const Point3D& x) const;
	Point3D		operator-(const Point3D& x) const;
	Point3D		operator/(const Point3D& x) const;
	Point3D		operator/(const float& x) { return *this / Point3D(x, x, x); }
	float&		operator[](int index);
	const float	operator[](int index) const;

	Point3D& normalize();
	Point3D& mult(float x);

	Point3D crossProduct(const Point3D& const x) const;

	Point3D unit() {

		return Point3D(sse_vec3.m128_f32[0] / length(), sse_vec3.m128_f32[1] / length(), sse_vec3.m128_f32[2] / length());
	}
	
	float dot(const Point3D& p1) const;
	float length() const;

	float length(Point3D p) const { return p.length(); } // blob monster

	void set(float x, float y, float z);

	Vector AsVector() const { return Vector(p[0], p[1], p[2]); }
	
private:
	
	/*DECL_ALIGN(4)*/ union
	{
		__m128 sse_vec3;
		float p[4];
	};
};