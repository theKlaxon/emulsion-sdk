#pragma once
#include "utlvector.h"
#include <xmmintrin.h>

class Point3D {
public:

	Point3D();
	Point3D(float x, float y, float z);

	Point3D		operator*(float x) const;
	Point3D		operator+(const Point3D& x) const;
	Point3D		operator-(const Point3D& x) const;
	Point3D		operator/(const Point3D& x) const;
	float*		operator[](int index);
	const float	operator[](int index) const;

	Point3D* normalize();
	Point3D* mult(float x);
	Point3D* crossProduct(const Point3D& x);
	Point3D* unit();// { Vector u = this->AsVector() / length(); return Point3D(u.x, u.y, u.z); }
	
	float dot(const Point3D& p1) const;
	float length() const;

	void set(float x, float y, float z);

	Vector AsVector() const { return Vector(p[0], p[1], p[2]); }
private:

	//float x, y, z;
	/*DECL_ALIGN(4)*/ union
	{
		__m128 sse_vec3;
		float p[4];
	};
};
