// == Valve Copyright somthinorother 2077 ==
// 
// Dis-fucking-claimer! This is not my code, this is valve code thats  
// been partially reconstructed in order to preserve it's functionality
// while at the same time avoiding outdated references in static
// library obj files. -Klax
// 
// Blobulator Point3D - Alien Swarm/particles.lib/(multiple files).obj
// =========================================
#include "cbase.h"
#include "iblob_renderer.h"
#include "Point3D.h"
//
//Point3D::Point3D() {
//	sse_vec3.m128_f32[3] = 0; // is this even correct??
//	return;
//}
//
//Point3D::Point3D(float x, float y, float z) {
//	sse_vec3.m128_f32[0] = x;
//	sse_vec3.m128_f32[1] = y;
//	sse_vec3.m128_f32[2] = z;
//	sse_vec3.m128_f32[3] = 0;
//	return;
//}
//
//Point3D Point3D::operator*(float x) const {
//	return Point3D(
//		sse_vec3.m128_f32[0] * x, 
//		sse_vec3.m128_f32[1] * x,
//		sse_vec3.m128_f32[2] * x);
//}
//
//Point3D Point3D::operator+(const Point3D& x) const {
//	return Point3D(
//		sse_vec3.m128_f32[0] + x.sse_vec3.m128_f32[0], 
//		sse_vec3.m128_f32[1] + x.sse_vec3.m128_f32[1], 
//		sse_vec3.m128_f32[2] + x.sse_vec3.m128_f32[2]);
//}
//
//Point3D Point3D::operator-(const Point3D& x) const {
//	return Point3D(
//		sse_vec3.m128_f32[0] - x.sse_vec3.m128_f32[0], 
//		sse_vec3.m128_f32[1] - x.sse_vec3.m128_f32[1], 
//		sse_vec3.m128_f32[2] - x.sse_vec3.m128_f32[2]);
//}
//
//Point3D Point3D::operator/(const Point3D& x) const {
//	return Point3D(
//		sse_vec3.m128_f32[0] / x.sse_vec3.m128_f32[0],
//		sse_vec3.m128_f32[1] / x.sse_vec3.m128_f32[1],
//		sse_vec3.m128_f32[2] / x.sse_vec3.m128_f32[2]);
//}
//
//float* Point3D::operator[](int index) {
//	return (float*)&sse_vec3.m128_f32[index];
//}
//
//const float Point3D::operator[](int index) const {
//	return sse_vec3.m128_f32[index];
//}

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

//Point3D* Point3D::mult(float x) {
//	sse_vec3.m128_f32[0] *= x;
//	sse_vec3.m128_f32[1] *= x;
//	sse_vec3.m128_f32[2] *= x;
//
//	return this;
//}

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

// TODO: find this somewhere in source 2007 maybe?
Point3D* Point3D::unit() {
	return &Point3D(sse_vec3.m128_f32[0] / length(), sse_vec3.m128_f32[1] / length(), sse_vec3.m128_f32[2] / length());
}
//
//float Point3D::dot(const Point3D& x) const {
//	return 
//		x.sse_vec3.m128_f32[0] * sse_vec3.m128_f32[0] +
//		x.sse_vec3.m128_f32[1] * sse_vec3.m128_f32[1] +
//		x.sse_vec3.m128_f32[2] * sse_vec3.m128_f32[2];
//}
//
//float Point3D::length() const {
//	return sqrt(
//		sse_vec3.m128_f32[0] * sse_vec3.m128_f32[0] +
//		sse_vec3.m128_f32[1] * sse_vec3.m128_f32[1] +
//		sse_vec3.m128_f32[2] * sse_vec3.m128_f32[0]);
//}
//
//void Point3D::set(float x, float y, float z) {
//	sse_vec3.m128_f32[0] = x;
//	sse_vec3.m128_f32[1] = y;
//	sse_vec3.m128_f32[2] = z;
//}