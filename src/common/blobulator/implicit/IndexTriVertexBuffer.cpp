#include "cbase.h"
#include "IndexTriVertexBuffer.h"

IndexTriVertexBuffer::IndexTriVertexBuffer() {
	// short m_s0 = 1;	// + 0
	// short m_s1 = 0;	// + 2
	// int m_n0 = 0;	// + 4
	// int m_s0 = 0;	// + 8

	m_vbId0 = 1;
}

IndexTriVertexBuffer::~IndexTriVertexBuffer() {

}

void IndexTriVertexBuffer::beginFrame(void* pVoid) {

	float uVar1;
	CMeshBuilder* pMeshBuilder;

}

void IndexTriVertexBuffer::beginCube(unsigned char* pChar) {

}

void IndexTriVertexBuffer::beginMesh() {

}

void IndexTriVertexBuffer::beginTile(ImpTile* pTile) {

}

void IndexTriVertexBuffer::beginVertex(vbId_t* pVertex) {

}

void IndexTriVertexBuffer::endFrame() {

}

void IndexTriVertexBuffer::endCube() {

}

void IndexTriVertexBuffer::endMesh() {

}

void IndexTriVertexBuffer::endTile() {

}

void IndexTriVertexBuffer::endVertex(const vbId_t* pVertex) {

}

void IndexTriVertexBuffer::drawVertex(const vbId_t* pVertex) {

}

void IndexTriVertexBuffer::init() {
	return; // as per the ghidra :/
}

bool IndexTriVertexBuffer::isIdValid(const vbId_t* pVertex) {
	return pVertex->id == m_vbId0;
}