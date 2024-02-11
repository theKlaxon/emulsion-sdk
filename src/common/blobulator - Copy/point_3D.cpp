//==== Blobulator. Copyright © 2004-2007, Rosware LLC, All rights reserved. ====//
#include "cbase.h"
#include "iblob_renderer.h"
#include "Point3D.h"

Point3D* Point3D::normalize() {

	float fVar1 = sse_vec3.m128_f32[1];
	float fVar2 = sse_vec3.m128_f32[0];
	float fVar3 = sse_vec3.m128_f32[2];
	float fVar4 = sqrt(fVar2 * fVar2 + fVar1 * fVar1 + fVar3 * fVar3);

	//if (fVar4 == ___real@00000000) { // assuming this means 0 cause it's all 0 (duh)
	if (fVar4 == 0.0f) {
		fVar4 = g_pReal3f800000;
	}
	fVar4 = g_pReal3f800000 / fVar4;

	sse_vec3.m128_f32[0] = fVar4 * fVar2;
	sse_vec3.m128_f32[1] = fVar4 * fVar1;
	sse_vec3.m128_f32[2] = fVar4 * fVar3;

	return this;
}

Point3D* Point3D::crossProduct(const Point3D& x) {
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

	return &in_stack_00000008;
}

Point3D* Point3D::unit() {
	return &Point3D(sse_vec3.m128_f32[0] / length(), sse_vec3.m128_f32[1] / length(), sse_vec3.m128_f32[2] / length());
}
