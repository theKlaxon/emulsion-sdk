//==== Blobulator. Copyright © 2004-2007, Rosware LLC, All rights reserved. ====//
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
	float&		operator[](int index);
	const float	operator[](int index) const;

	Point3D& normalize();
	Point3D& mult(float x);

	Point3D crossProduct(const Point3D& x) {
		Point3D in_stack_00000008 = x;

		float fVar1 = sse_vec3.m128_f32[2];
		float fVar2 = sse_vec3.m128_f32[1];
		float fVar3 = in_stack_00000008.sse_vec3.m128_f32[2];
		float fVar4 = in_stack_00000008.sse_vec3.m128_f32[1];
		float fVar5 = in_stack_00000008.sse_vec3.m128_f32[0];

		//*(float*)(&in_stack_00000008 + 0xc) = 0; // p?????
		in_stack_00000008.sse_vec3.m128_f32[3] = 0;

		sse_vec3.m128_f32[0] = fVar2 * fVar3 - fVar1 * fVar4;
		float fVar6 = sse_vec3.m128_f32[0];// *(float*)this;
		sse_vec3.m128_f32[1] = fVar5 * fVar1 - fVar6 * fVar3;
		sse_vec3.m128_f32[2] = fVar6 * fVar4 - fVar5 * fVar2;

		return in_stack_00000008;
	}

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